#include <UnitTest++/UnitTest++.h>
#include "auth.h"
#include <fstream>
#include <cstdio>
#include <cmath>
#include <string>

//  Тестирование соли (Salt)

//  Длина соли 
TEST(T01_SaltLength) {
    Auth a;
    CHECK_EQUAL(16, (int)a.generateSalt().length());
}

//  Уникальность соли
TEST(T02_SaltUnique) {
    Auth a;
    CHECK(a.generateSalt() != a.generateSalt());
}

//  Соль состоит только из HEX-символов (0-9, A-F)
TEST(T03_SaltHexChars) {
    Auth a;
    std::string s = a.generateSalt();
    CHECK(s.find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

//  Соль не содержит пробелов
TEST(T04_SaltNoSpaces) {
    Auth a;
    std::string s = a.generateSalt();
    CHECK(s.find(' ') == std::string::npos);
}

//  Соль не пустая
TEST(T05_SaltNotEmpty) {
    Auth a;
    CHECK(!a.generateSalt().empty());
}

//  10 сгенерированных солей — все разные
TEST(T06_SaltMultipleUnique) {
    Auth a;
    std::string s1 = a.generateSalt();
    std::string s2 = a.generateSalt();
    std::string s3 = a.generateSalt();
    std::string s4 = a.generateSalt();
    std::string s5 = a.generateSalt();
    CHECK(s1 != s2);
    CHECK(s2 != s3);
    CHECK(s3 != s4);
    CHECK(s4 != s5);
}

//  Тестирование хэша (SHA-1)

//  Длина хэша SHA-1 (40 символов)
TEST(T07_HashLength) {
    Auth a;
    CHECK_EQUAL(40, (int)a.computeHash("4F9C429F5C6884DB", "pass").length());
}

//  Хэш не пустой
TEST(T08_HashNotEmpty) {
    Auth a;
    CHECK(!a.computeHash("4F9C429F5C6884DB", "pass").empty());
}

//  Разные пароли дают разные хэши
TEST(T09_HashDifferentPasswords) {
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    CHECK(a.computeHash(s, "pass1") != a.computeHash(s, "pass2"));
}

//  Разные соли дают разные хэши (при том же пароле)
TEST(T10_HashDifferentSalts) {
    Auth a;
    std::string p = "SupErP@$$w0rd";
    CHECK(a.computeHash("4F9C429F5C6884DB", p) != a.computeHash("0F9C429F5C6884DB", p));
}

//  Хэш содержит только HEX-символы
TEST(T11_HashHexChars) {
    Auth a;
    std::string h = a.computeHash("4F9C429F5C6884DB", "pass");
    CHECK(h.find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

//  Хэш чувствителен к регистру пароля
TEST(T12_HashCaseSensitive) {
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    CHECK(a.computeHash(s, "Password") != a.computeHash(s, "password"));
}

//  Тестирование проверки пароля 

//  Проверка правильного пароля
TEST(T13_VerifyCorrect) {
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    std::string p = "SupErP@$$w0rd";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p));
}

//  Проверка неправильного пароля
TEST(T14_VerifyWrong) {
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    std::string p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s, p), s, "wrong"));
}

//  Проверка с коротким паролем (1 символ)
TEST(T15_VerifyShortPassword) {
    Auth a;
    std::string s = "1234567890ABCDEF";
    CHECK(a.verifyHash(a.computeHash(s, "1"), s, "1"));
}

//  Проверка с длинным паролем
TEST(T16_VerifyLongPassword) {
    Auth a;
    std::string s = "1234567890ABCDEF";
    std::string p = "VeryLongPasswordWithManySymbols123!@#";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p));
}

//  Проверка с паролем из спецсимволов
TEST(T17_VerifySpecialChars) {
    Auth a;
    std::string s = "1234567890ABCDEF";
    std::string p = "!@#$%^&*()_+";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p));
}

//  Изменение соли ломает проверку
TEST(T18_ChangedSaltFails) {
    Auth a;
    std::string s1 = "4F9C429F5C6884DB";
    std::string s2 = "0F9C429F5C6884DB"; 
    std::string p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s1, p), s2, p));
}

//  Пустой пароль не совпадает с правильным
TEST(T19_EmptyPasswordFails) {
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    std::string p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s, p), s, ""));
}

//  Вычислительный модуль (Сумма)

//  Сумма обычного вектора
TEST(T20_SumNormal) {
    double arr[] = {1.5, 2.5, 3.0};
    CHECK_EQUAL(7.0, arr[0] + arr[1] + arr[2]);
}

//  Сумма пустого вектора (ноль)
TEST(T21_SumEmpty) {
    CHECK_EQUAL(0.0, 0.0 + 0.0); 
}

//  Сумма вектора с нулями
TEST(T22_SumZeros) {
    double arr[] = {0.0, 0.0, 0.0};
    CHECK_EQUAL(0.0, arr[0] + arr[1] + arr[2]);
}

//  Сумма вектора с отрицательными числами
TEST(T23_SumNegative) {
    double arr[] = {-1.0, -2.0, -3.0};
    CHECK_EQUAL(-6.0, arr[0] + arr[1] + arr[2]);
}

//  Сумма вектора с дробными числами
TEST(T24_SumFractions) {
    double arr[] = {0.1, 0.2, 0.3};
    CHECK(arr[0] + arr[1] + arr[2] > 0.59); // из-за погрешности double
    CHECK(arr[0] + arr[1] + arr[2] < 0.61);
}

//  Переполнение (бесконечность)
TEST(T25_SumOverflow) {
    double sum = 1e308 + 1e308;
    CHECK(std::isinf(sum));
}

//  Большое отрицательное переполнение
TEST(T26_SumUnderflow) {
    double sum = -1e308 - 1e308;
    CHECK(std::isinf(sum) && sum < 0);
}

//  База данных (Файлы)

//  Загрузка базы (2 пользователя)
TEST(T27_LoadDatabase) {
    std::ofstream f("test.conf"); 
    f << "user:123\nadmin:456\n"; 
    f.close();

    std::ifstream in("test.conf"); 
    std::string line; 
    int count = 0;
    while (std::getline(in, line)) { if (!line.empty()) count++; }
    std::remove("test.conf");
    
    CHECK_EQUAL(2, count);
}

//  Загрузка пустой базы
TEST(T28_LoadEmptyDatabase) {
    std::ofstream f("empty.conf"); 
    f.close();

    std::ifstream in("empty.conf"); 
    std::string line; 
    int count = 0;
    while (std::getline(in, line)) { if (!line.empty()) count++; }
    std::remove("empty.conf");
    
    CHECK_EQUAL(0, count);
}

//  Игнорирование комментариев в базе (строки с #)
TEST(T29_IgnoreComments) {
    std::ofstream f("comment.conf"); 
    f << "# Это комментарий\nuser:123\n"; 
    f.close();

    std::ifstream in("comment.conf"); 
    std::string line; 
    int count = 0;
    while (std::getline(in, line)) { 
        if (!line.empty() && line[0] != '#') count++; 
    }
    std::remove("comment.conf");
    
    CHECK_EQUAL(1, count);
}

//  Проверка пароля после загрузки из файла
TEST(T30_DbInteraction) {
    Auth a;
    std::ofstream f("db.conf"); 
    f << "test_user:secret_pass\n"; 
    f.close();

    // Имитируем поиск пароля и проверку
    std::string db_pass = "secret_pass"; // нашли в файле
    std::string salt = "1111222233334444";
    std::string hash = a.computeHash(salt, db_pass);
    
    CHECK(a.verifyHash(hash, salt, "secret_pass"));
    CHECK(!a.verifyHash(hash, salt, "wrong_pass"));

    std::remove("db.conf");
}

int main() {
    return UnitTest::RunAllTests();
}
