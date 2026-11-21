/**
 * @file main.cpp
 * @brief Точка входа в программу сервера.
 * @details Создает объект Server, разбирает параметры и запускает сервер.
 */

#include <iostream>
#include "server.h"

/**
 * @brief Главная функция программы.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return int Код возврата (0 - успех, 1 - ошибка).
 * 
 * Алгоритм:
 * 1. Создать объект Server
 * 2. Разобрать параметры командной строки
 * 3. Если параметры корректны, запустить сервер
 * 4. Обработать исключения
 */
int main(int argc, char** argv) {
    Server server;
    if (server.parseArgs(argc, argv)) {
        try {
            server.start();
        } catch (std::exception& e) {
            std::cerr << "Критическая ошибка: " << e.what() << "\n";
        }
    }
    return 0;
}
