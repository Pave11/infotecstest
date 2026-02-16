#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

/**
 * @brief Уровень логирования сообщения.
 */
enum class LogLevel
{
    Info,
    Error
};

/**
 * @brief Лёгкий логгер для консоли.
 *
 * Сообщение выводится в зависимости от уровня:
 * - Info  → стандартный вывод
 * - Error → stderr
 *
 * @param level Уровень логирования
 * @param message Сообщение
 */
inline void logMessage(LogLevel level, const std::string& message)
{
    switch (level)
    {
    case LogLevel::Info:
        std::cout << "[INFO] " << message << std::endl;
        break;
    case LogLevel::Error:
        std::cerr << "[ERROR] " << message << std::endl;
        break;
    }
}

#endif // LOGGER_H
