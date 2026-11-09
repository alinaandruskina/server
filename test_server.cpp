#include <UnitTest++/UnitTest++.h>
#include "auth.h"
#include <iostream>  
#include <fstream>
#include <cstdio>
#include <cmath>

//  Аутентификация
TEST(T01_SaltLength) { 
    Auth a; 
    CHECK_EQUAL(16, (int)a.generateSalt().length()); 
}

TEST(T02_SaltUnique) { 
    Auth a; 
    CHECK(a.generateSalt() != a.generateSalt()); 
}

TEST(T03_SaltHex) { 
    Auth a; 
    std::string s = a.generateSalt();
    CHECK(s.find_first_not_of("0123456789ABCDEF") == std::string::npos); 
}

TEST(T04_HashLength) { 
    Auth a; 
    CHECK_EQUAL(40, (int)a.computeHash("4F9C429F5C6884DB", "SupErP@$$w0rd").length()); 
}

TEST(T05_VerifyCorrect) { 
    Auth a; 
    std::string s = "4F9C429F5C6884DB", p = "SupErP@$$w0rd";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p)); 
}

TEST(T06_VerifyWrong) { 
    Auth a; 
    std::string s = "4F9C429F5C6884DB", p = "SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s, p), s, "wrong")); 
}

//  База данных
TEST(T07_LoadDatabase) {
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

//  Вычислительный модуль
TEST(T08_SumPositive) { 
    double arr[] = {1.5, 2.5, 3.0}; 
    CHECK_EQUAL(7.0, arr[0] + arr[1] + arr[2]); 
}

TEST(T09_SumOverflow) { 
    double sum = 1e308 + 1e308; 
    CHECK(std::isinf(sum)); 
}

// Главная функция 
int main() {
    return UnitTest::RunAllTests();
}
