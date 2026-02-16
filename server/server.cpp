#include "server.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <csignal>
#include <iostream>

std::atomic<bool> Server::running_(true);

Server::Server(int port, const std::string& filePath, size_t chunkSize)
    : port_(port), filePath_(filePath), chunkSize_(chunkSize), serverSocket_(-1)
{
}

void Server::signalHandler(int signum)
{
    logMessage(LogLevel::Info, "Получен сигнал остановки сервера");
    running_ = false;
}

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

        if (!running_) break; // если сигнал, выходим

        if (clientSock < 0)
        {
            if (running_) logMessage(LogLevel::Error, "Ошибка accept");
            continue;
        }

        logMessage(LogLevel::Info, "Клиент подключен");

        // Создаём поток для клиента
        clientThreads_.emplace_back(&Server::handleClient, this, clientSock);
        clientThreads_.back().detach();
    }

    logMessage(LogLevel::Info, "Сервер завершает работу...");

    if (serverSocket_ >= 0)
        close(serverSocket_);
}

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
    size_t totalSize = file.tellg();
    file.seekg(0, std::ios::beg);

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
            std::cout << "\r<передано " << sentBytes << " байт / " << totalSize << " байт>" << std::flush;
        }
    }

    logMessage(LogLevel::Info, "\nФайл отправлен клиенту.");
    delete[] buffer;
    file.close();
    close(clientSocket);
}
