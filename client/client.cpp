#include "client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

Client::Client(const std::string& serverIp,
               const std::string& serverFile,
               const std::string& localFile,
               int port,
               size_t chunkSize)
    : serverIp_(serverIp),
      serverFile_(serverFile),
      localFile_(localFile),
      port_(port),
      chunkSize_(chunkSize)
{
}

bool Client::parseSrc(const std::string& src, std::string& ip, std::string& path)
{
    size_t pos = src.find(':');
    if (pos == std::string::npos) return false;
    ip = src.substr(0, pos);
    path = src.substr(pos + 1);
    return true;
}

void Client::run()
{
    // Создаём сокет
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        logMessage(LogLevel::Error, "Ошибка создания сокета");
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    if (inet_pton(AF_INET, serverIp_.c_str(), &serverAddr.sin_addr) <= 0)
    {
        logMessage(LogLevel::Error, "Неверный IP-адрес");
        return;
    }

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        logMessage(LogLevel::Error, "Не удалось подключиться к серверу");
        return;
    }

    // Временный файл
    std::string tmpFile = localFile_ + ".tmp";
    std::ofstream out(tmpFile, std::ios::binary);
    if (!out.is_open())
    {
        logMessage(LogLevel::Error, "Не удалось создать файл " + tmpFile);
        close(sock);
        return;
    }

    char* buffer = new char[chunkSize_];
    size_t totalBytes = 0;
    ssize_t n;

    // Получаем данные от сервера
    while ((n = recv(sock, buffer, chunkSize_, 0)) > 0)
    {
        out.write(buffer, n);
        totalBytes += n;
        std::cout << "\r<передано " << totalBytes << " байт>" << std::flush;
    }

    logMessage(LogLevel::Info, "\nПриём файла завершён.");

    delete[] buffer;
    out.close();
    close(sock);

    // Переименовываем временный файл в конечный
    if (totalBytes > 0)
    {
        if (std::rename(tmpFile.c_str(), localFile_.c_str()) != 0)
        {
            logMessage(LogLevel::Error, "Не удалось переименовать файл");
            return;
        }
        logMessage(LogLevel::Info, "Файл сохранён как " + localFile_);
    }
    else
    {
        std::remove(tmpFile.c_str());
        logMessage(LogLevel::Error, "Файл не получен");
    }
}
