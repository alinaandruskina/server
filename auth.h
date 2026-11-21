#pragma once
#include <string>

/**
 * @file auth.h
 * @brief Заголовочный файл модуля аутентификации.
 * @details Содержит класс Auth для генерации соли и проверки хешей SHA-1.
 * @author alinaandruskina
 * @version 1.0
 * @date 20.11.2026
 */

/**
 * @class Auth
 * @brief Класс для реализации криптографической аутентификации.
 * @details Использует библиотеку Crypto++ для генерации случайной соли 
 * и вычисления хеш-функции SHA-1 от конкатенации (соль + пароль).
 * Схема аутентификации: сервер генерирует соль, клиент вычисляет хеш 
 * от соли и пароля, сервер проверяет хеш.
 */
class Auth {
public:
    /**
     * @brief Генерирует случайную соль заданной длины.
     * @details Использует криптографически стойкий генератор псевдослучайных чисел 
     * (AutoSeededRandomPool из Crypto++). Соль генерируется как 8 случайных байт 
     * и преобразуется в 16-символьную HEX-строку.
     * @return std::string Строка соли в HEX-формате (16 символов, 0-9, A-F).
     * @throw std::runtime_error Если генератор случайных чисел недоступен.
     */
    std::string generateSalt();

    /**
     * @brief Вычисляет хеш SHA-1 от соединения соли и пароля.
     * @details Формула: HASH = SHA1(salt + password). Результат преобразуется 
     * в 40-символьную HEX-строку.
     * @param salt Соль (HEX-строка, 16 символов).
     * @param password Пароль пользователя (plain text).
     * @return std::string Хеш в виде HEX-строки (40 символов).
     * @throw std::runtime_error Если библиотека Crypto++ недоступна.
     */
    std::string computeHash(const std::string& salt, const std::string& password);
    
    /**
     * @brief Проверяет корректность хеша.
     * @details Вычисляет хеш от заданных соли и пароля и сравнивает с переданным хешем.
     * Используется константное время сравнения для защиты от атак по времени.
     * @param hash Ожидаемый хеш (HEX-строка).
     * @param salt Соль (HEX-строка).
     * @param password Пароль для проверки.
     * @return true Если хеши совпадают.
     * @return false Если хеши не совпадают.
     */
    bool verifyHash(const std::string& hash, const std::string& salt, const std::string& password);
};
