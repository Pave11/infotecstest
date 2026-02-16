#ifndef CLIENT_H
#define CLIENT_H

#include "logger.h"
#include <string>
#include <cstddef>

/**
 * @brief Класс TCP-клиента, который получает файл с сервера.
 */
class Client
{
public:
    /**
     * @brief Конструктор клиента
     * @param serverIp IP сервера
     * @param serverFile Путь к файлу на сервере
     * @param localFile Путь для сохранения на клиенте
     * @param port Порт сервера
     * @param chunkSize Размер порции передачи
     */
    Client(const std::string& serverIp,
           const std::string& serverFile,
           const std::string& localFile,
           int port = 4040,
           size_t chunkSize = 1024);

    /**
     * @brief Запустить получение файла (блокирующая функция)
     */
    void run();

private:
    std::string serverIp_;
    std::string serverFile_;
    std::string localFile_;
    int port_;
    size_t chunkSize_;

    /**
     * @brief Разбор аргумента -src формата <IP>:<путь>
     */
    static bool parseSrc(const std::string& src, std::string& ip, std::string& path);
};

#endif // CLIENT_H
