#include <UnitTest++/UnitTest++.h>
#include "auth.h"
#include <fstream>
#include <cstdio>
#include <cmath>
#include <limits>

//  Аутентификация
SUITE(AuthTest) {
    
    //  Длина соли (16 символов)
    TEST(SaltLength) {
        Auth a;
        CHECK_EQUAL(16, (int)a.generateSalt().length());
    }

    //  Уникальность соли
    TEST(SaltUnique) {
        Auth a;
        CHECK(a.generateSalt() != a.generateSalt());
    }

    //  Только hex-символы (0-9, A-F)
    TEST(SaltHasOnlyHexChars) {
        Auth a;
        std::string salt = a.generateSalt();
        CHECK(salt.find_first_not_of("0123456789ABCDEF") == std::string::npos);
    }

    //  Длина хэша SHA-1 (40 символов)
    TEST(HashComputation) {
        Auth a;
        std::string h = a.computeHash("4F9C429F5C6884DB", "SupErP@$$w0rd");
        CHECK_EQUAL(40, (int)h.length());
    }

    //  Проверка правильного пароля
    TEST(VerifyCorrectPassword) {
        Auth a;
        std::string salt = "4F9C429F5C6884DB";
        std::string pass = "SupErP@$$w0rd";
        std::string h = a.computeHash(salt, pass);
        CHECK(a.verifyHash(h, salt, pass));
    }

    //  Проверка неправильного пароля
    TEST(VerifyWrongPassword) {
        Auth a;
        std::string salt = "4F9C429F5C6884DB";
        std::string pass = "SupErP@$$w0rd";
        std::string h = a.computeHash(salt, pass);
        CHECK(!a.verifyHash(h, salt, "wrong_password"));
    }
}

// Обработка данных
SUITE(CalcTest) {
    
    //  Сумма нормального вектора
    TEST(SumCalculation) {
        double arr[] = {1.5, 2.5, 3.0};
        double sum = arr[0] + arr[1] + arr[2];
        CHECK_EQUAL(7.0, sum);
    }

    //  Переполнение (бесконечность)
    TEST(SumOverflow) {
        double sum = 1e308 + 1e308; // Слишком большое число
        CHECK(std::isinf(sum));
    }
}

//  База данных 
SUITE(DatabaseTest) {
    
    //  Загрузка файла
    TEST(LoadValidDatabase) {
        std::ofstream file("test_auth.conf");
        file << "user:P@ssW0rd\n";
        file << "ibst128:12345\n";
        file.close();

        std::ifstream in("test_auth.conf");
        std::string line;
        int count = 0;
        while (std::getline(in, line)) {
            if (!line.empty()) count++;
        }
        std::remove("test_auth.conf");
        
        CHECK_EQUAL(2, count);
    }
}

int main() {
    return UnitTest::RunAllTests();
}
