#ifndef SERVER_H
#define SERVER_H

#include "logger.h"
#include <string>
#include <vector>
#include <thread>
#include <atomic>

/**
 * @brief Класс TCP-сервера, который отдаёт файл клиентам.
 */
class Server
{
public:
    /**
     * @brief Конструктор сервера
     * @param port Порт для прослушивания
     * @param filePath Путь к файлу для отдачи
     * @param chunkSize Размер порции передачи
     */
    Server(int port, const std::string& filePath, size_t chunkSize = 1024);

    /**
     * @brief Запустить сервер (блокирующая функция)
     */
    void run();

private:
    int port_;
    std::string filePath_;
    size_t chunkSize_;
    int serverSocket_;
    std::vector<std::thread> clientThreads_;

    /**
     * @brief Настройка сокета и запуск listen
     */
    void setupSocket();

    /**
     * @brief Обработка одного клиента
     * @param clientSocket Сокет клиента
     */
    void handleClient(int clientSocket);

    static std::atomic<bool> running_; // флаг работы сервера
    static void signalHandler(int signum); // обработчик сигналов
};

#endif // SERVER_H
