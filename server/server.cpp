#include "server.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <csignal>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <cstdint>
#include <endian.h> 

// Статический флаг работы сервера
std::atomic<bool> Server::running_(true);

Server::Server(int port, const std::string& filePath, size_t chunkSize)
    : port_(port), filePath_(filePath), chunkSize_(chunkSize), serverSocket_(-1)
{
}

// Обработчик сигналов Ctrl+C / SIGTERM
void Server::signalHandler(int signum)
{
    logMessage(LogLevel::Info, "Получен сигнал остановки сервера");
    running_ = false;
}

// Настройка сокета сервера
void Server::setupSocket()
{
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0)
    {
        logMessage(LogLevel::Error, "Ошибка создания сокета");
        exit(1);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);

    if (bind(serverSocket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        logMessage(LogLevel::Error, "Ошибка привязки сокета к порту");
        exit(1);
    }

    if (listen(serverSocket_, 5) < 0)
    {
        logMessage(LogLevel::Error, "Ошибка запуска listen");
        exit(1);
    }

    logMessage(LogLevel::Info, "Сервер запущен на порту " + std::to_string(port_));
}

// Основной цикл сервера
void Server::run()
{
    setupSocket();

    // Вешаем обработчики сигналов
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    while (running_)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSocket_, (sockaddr*)&clientAddr, &clientLen);

        if (!running_) break; // если сигнал остановки, выходим

        if (clientSock < 0)
        {
            if (running_) logMessage(LogLevel::Error, "Ошибка accept");
            continue;
        }

        logMessage(LogLevel::Info, "Клиент подключен");

        // Создаём поток для обслуживания клиента
        clientThreads_.emplace_back(&Server::handleClient, this, clientSock);
        clientThreads_.back().detach();
    }

    logMessage(LogLevel::Info, "Сервер завершает работу...");

    if (serverSocket_ >= 0)
        close(serverSocket_);
}

// Обслуживание одного клиента
void Server::handleClient(int clientSocket)
{
    std::ifstream file(filePath_, std::ios::binary);
    if (!file.is_open())
    {
        logMessage(LogLevel::Error, "Не удалось открыть файл: " + filePath_);
        close(clientSocket);
        return;
    }

    file.seekg(0, std::ios::end);
    uint64_t totalSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Отправляем клиенту размер файла (сетевой порядок байт)
    uint64_t sizeNet = htobe64(totalSize);
    if (send(clientSocket, reinterpret_cast<char*>(&sizeNet), sizeof(sizeNet), 0) != sizeof(sizeNet))
    {
        logMessage(LogLevel::Error, "Не удалось отправить размер файла клиенту");
        close(clientSocket);
        return;
    }

    size_t sentBytes = 0;
    char* buffer = new char[chunkSize_];

    while (file)
    {
        file.read(buffer, chunkSize_);
        std::streamsize readBytes = file.gcount();

        if (readBytes > 0)
        {
            ssize_t n = send(clientSocket, buffer, readBytes, 0);
            if (n < 0)
            {
                logMessage(LogLevel::Error, "Ошибка отправки данных клиенту");
                break;
            }
            sentBytes += n;

            // Прогресс в процентах
            double percent = (double)sentBytes / totalSize * 100;
            std::cout << "\r<передано " << sentBytes << " / " << totalSize
                      << " байт (" << int(percent) << "%)>" << std::flush;
        }
    }

    logMessage(LogLevel::Info, "\nФайл отправлен клиенту.");
    delete[] buffer;
    file.close();
    close(clientSocket);
}
