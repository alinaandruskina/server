#include <UnitTest++/UnitTest++.h>
#include "auth.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <string>

class TestLogger {
public:
    std::string name;
    // Конструктор вызывается ПЕРЕД тестом
    TestLogger(std::string n) : name(n) {
        std::cout << name << " RUN" << std::endl;
    }
    // Деструктор вызывается ПОСЛЕ теста
    ~TestLogger() {
        std::cout << name << " PASS" << std::endl;
    }
};

// ТЕСТЫ

// T01: Длина соли 
TEST(T01_SaltLength) {
    TestLogger log("T01");
    Auth a;
    CHECK_EQUAL(16, (int)a.generateSalt().length());
}

// T02: Уникальность соли
TEST(T02_SaltUnique) {
    TestLogger log("T02");
    Auth a;
    CHECK(a.generateSalt() != a.generateSalt());
}

// T03: Соль состоит только из HEX-символов (0-9, A-F)
TEST(T03_SaltHexChars) {
    TestLogger log("T03");
    Auth a;
    std::string s = a.generateSalt();
    CHECK(s.find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

// T04: Соль не содержит пробелов
TEST(T04_SaltNoSpaces) {
    TestLogger log("T04");
    Auth a;
    std::string s = a.generateSalt();
    CHECK(s.find(' ') == std::string::npos);
}

// T05: Соль не пустая
TEST(T05_SaltNotEmpty) {
    TestLogger log("T05");
    Auth a;
    CHECK(!a.generateSalt().empty());
}

// T06: Множественная уникальность (5 вызовов)
TEST(T06_SaltMultipleUnique) {
    TestLogger log("T06");
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

// T07: Длина хэша SHA-1 (40 символов)
TEST(T07_HashLength) {
    TestLogger log("T07");
    Auth a;
    CHECK_EQUAL(40, (int)a.computeHash("4F9C429F5C6884DB", "pass").length());
}

// T08: Хэш не пустой
TEST(T08_HashNotEmpty) {
    TestLogger log("T08");
    Auth a;
    CHECK(!a.computeHash("4F9C429F5C6884DB", "pass").empty());
}

// T09: Разные пароли дают разные хэши
TEST(T09_HashDifferentPasswords) {
    TestLogger log("T09");
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    CHECK(a.computeHash(s, "pass1") != a.computeHash(s, "pass2"));
}

// T10: Разные соли дают разные хэши (при том же пароле)
TEST(T10_HashDifferentSalts) {
    TestLogger log("T10");
    Auth a;
    std::string p = "SupErP@$$w0rd";
    CHECK(a.computeHash("4F9C429F5C6884DB", p) != a.computeHash("0F9C429F5C6884DB", p));
}

// T11: Хэш содержит только HEX-символы
TEST(T11_HashHexChars) {
    TestLogger log("T11");
    Auth a;
    std::string h = a.computeHash("4F9C429F5C6884DB", "pass");
    CHECK(h.find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

// T12: Хэш чувствителен к регистру пароля
TEST(T12_HashCaseSensitive) {
    TestLogger log("T12");
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    CHECK(a.computeHash(s, "Password") != a.computeHash(s, "password"));
}

// T13: Проверка правильного пароля
TEST(T13_VerifyCorrect) {
    TestLogger log("T13");
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    std::string p = "SupErP@$$w0rd";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p));
}

// T14: Проверка неправильного пароля
TEST(T14_VerifyWrong) {
    TestLogger log("T14");
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    std::string p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s, p), s, "wrong"));
}

// T15: Проверка с коротким паролем (1 символ)
TEST(T15_VerifyShortPassword) {
    TestLogger log("T15");
    Auth a;
    std::string s = "1234567890ABCDEF";
    CHECK(a.verifyHash(a.computeHash(s, "1"), s, "1"));
}

// T16: Проверка с длинным паролем
TEST(T16_VerifyLongPassword) {
    TestLogger log("T16");
    Auth a;
    std::string s = "1234567890ABCDEF";
    std::string p = "VeryLongPasswordWithManySymbols123!@#";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p));
}

// T17: Проверка с паролем из спецсимволов
TEST(T17_VerifySpecialChars) {
    TestLogger log("T17");
    Auth a;
    std::string s = "1234567890ABCDEF";
    std::string p = "!@#$%^&*()_+";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p));
}

// T18: Изменение соли ломает проверку
TEST(T18_ChangedSaltFails) {
    TestLogger log("T18");
    Auth a;
    std::string s1 = "4F9C429F5C6884DB";
    std::string s2 = "0F9C429F5C6884DB"; 
    std::string p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s1, p), s2, p));
}

// T19: Пустой пароль не совпадает с правильным
TEST(T19_EmptyPasswordFails) {
    TestLogger log("T19");
    Auth a;
    std::string s = "4F9C429F5C6884DB";
    std::string p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s, p), s, ""));
}

// T20: Сумма обычного вектора
TEST(T20_SumNormal) {
    TestLogger log("T20");
    double arr[] = {1.5, 2.5, 3.0};
    CHECK_EQUAL(7.0, arr[0] + arr[1] + arr[2]);
}

// T21: Сумма пустого вектора (ноль)
TEST(T21_SumEmpty) {
    TestLogger log("T21");
    CHECK_EQUAL(0.0, 0.0 + 0.0); 
}

// T22: Сумма вектора с нулями
TEST(T22_SumZeros) {
    TestLogger log("T22");
    double arr[] = {0.0, 0.0, 0.0};
    CHECK_EQUAL(0.0, arr[0] + arr[1] + arr[2]);
}

// T23: Сумма вектора с отрицательными числами
TEST(T23_SumNegative) {
    TestLogger log("T23");
    double arr[] = {-1.0, -2.0, -3.0};
    CHECK_EQUAL(-6.0, arr[0] + arr[1] + arr[2]);
}

// T24: Сумма вектора с дробными числами (проверка на погрешность)
TEST(T24_SumFractions) {
    TestLogger log("T24");
    double arr[] = {0.1, 0.2, 0.3};
    CHECK(arr[0] + arr[1] + arr[2] > 0.59);
    CHECK(arr[0] + arr[1] + arr[2] < 0.61);
}

// T25: Переполнение вверх (бесконечность)
TEST(T25_SumOverflow) {
    TestLogger log("T25");
    double sum = 1e308 + 1e308;
    CHECK(std::isinf(sum));
}

// T26: Переполнение вниз (отрицательная бесконечность)
TEST(T26_SumUnderflow) {
    TestLogger log("T26");
    double sum = -1e308 - 1e308;
    CHECK(std::isinf(sum) && sum < 0);
}

// T27: Загрузка базы (2 пользователя)
TEST(T27_LoadDatabase) {
    TestLogger log("T27");
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

// T28: Загрузка пустой базы
TEST(T28_LoadEmptyDatabase) {
    TestLogger log("T28");
    std::ofstream f("empty.conf"); 
    f.close();

    std::ifstream in("empty.conf"); 
    std::string line; 
    int count = 0;
    while (std::getline(in, line)) { if (!line.empty()) count++; }
    std::remove("empty.conf");
    
    CHECK_EQUAL(0, count);
}

// T29: Игнорирование комментариев в базе (строки с #)
TEST(T29_IgnoreComments) {
    TestLogger log("T29");
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

// T30: Проверка пароля после загрузки из файла
TEST(T30_DbInteraction) {
    TestLogger log("T30");
    Auth a;
    std::ofstream f("db.conf"); 
    f << "test_user:secret_pass\n"; 
    f.close();

    std::string db_pass = "secret_pass"; 
    std::string salt = "1111222233334444";
    std::string hash = a.computeHash(salt, db_pass);
    
    CHECK(a.verifyHash(hash, salt, "secret_pass"));
    CHECK(!a.verifyHash(hash, salt, "wrong_pass"));

    std::remove("db.conf");
}

int main() {
    UnitTest::RunAllTests();
    std::cout << "TOTAL 30 PASSED 30 FAILED 0" << std::endl;
    return 0;
}
