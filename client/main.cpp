#include "client.h"
#include <iostream>
#include <string>

// Простая функция для разбора IP и пути из -src
bool parseSrc(const std::string& src, std::string& ip, std::string& path)
{
    size_t pos = src.find(':');
    if (pos == std::string::npos) return false;
    ip = src.substr(0, pos);
    path = src.substr(pos + 1);
    return true;
}

int main(int argc, char* argv[])
{
    std::string srcArg;
    std::string dstPath;
    int port = 4040;
    size_t chunkSize = 1024;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-src" && i + 1 < argc)
        {
            srcArg = argv[++i];
        }
        else if (arg == "-dst" && i + 1 < argc)
        {
            dstPath = argv[++i];
        }
        else if (arg == "-p" && i + 1 < argc)
        {
            port = std::stoi(argv[++i]);
        }
        else if (arg == "-chunk" && i + 1 < argc)
        {
            chunkSize = std::stoul(argv[++i]);
        }
    }

    if (srcArg.empty() || dstPath.empty())
    {
        std::cerr << "Использование: client_app -src <IP>:<путь> -dst <локальный путь> [-p порт] [-chunk размер]\n";
        return 1;
    }

    std::string serverIp, serverFile;
    if (!parseSrc(srcArg, serverIp, serverFile))
    {
        std::cerr << "Неверный формат -src. Пример: 127.0.0.1:/tmp/file\n";
        return 1;
    }

    Client client(serverIp, serverFile, dstPath, port, chunkSize);
    client.run();

    return 0;
}
