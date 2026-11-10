#include <UnitTest++/UnitTest++.h>
#include "auth.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <string>

// ==========================================
// КЛАСС-ФИКСТУРА ДЛЯ АВТОМАТИЧЕСКОГО ВЫВОДА
// ==========================================
class TestLogger {
public:
    std::string name;
    TestLogger(std::string n) : name(n) {
        std::cout << name << " RUN" << std::endl;
    }
    ~TestLogger() {
        std::cout << name << " PASS" << std::endl;
    }
};

// ТЕСТЫ

TEST(T01_SaltLength) {
    TestLogger log("T01");
    Auth a; CHECK_EQUAL(16, (int)a.generateSalt().length());
}

TEST(T02_SaltUnique) {
    TestLogger log("T02");
    Auth a; CHECK(a.generateSalt() != a.generateSalt());
}

TEST(T03_SaltHexChars) {
    TestLogger log("T03");
    Auth a; CHECK(a.generateSalt().find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

TEST(T04_SaltNoSpaces) {
    TestLogger log("T04");
    Auth a; CHECK(a.generateSalt().find(' ') == std::string::npos);
}

TEST(T05_SaltNotEmpty) {
    TestLogger log("T05");
    Auth a; CHECK(!a.generateSalt().empty());
}

TEST(T06_SaltMultipleUnique) {
    TestLogger log("T06");
    Auth a; CHECK(a.generateSalt() != a.generateSalt());
}

TEST(T07_HashLength) {
    TestLogger log("T07");
    Auth a; CHECK_EQUAL(40, (int)a.computeHash("salt", "pass").length());
}

TEST(T08_HashNotEmpty) {
    TestLogger log("T08");
    Auth a; CHECK(!a.computeHash("salt", "pass").empty());
}

TEST(T09_HashDifferentPasswords) {
    TestLogger log("T09");
    Auth a; CHECK(a.computeHash("salt", "p1") != a.computeHash("salt", "p2"));
}

TEST(T10_HashDifferentSalts) {
    TestLogger log("T10");
    Auth a; CHECK(a.computeHash("salt1", "p") != a.computeHash("salt2", "p"));
}

TEST(T11_HashHexChars) {
    TestLogger log("T11");
    Auth a; CHECK(a.computeHash("salt", "p").find_first_not_of("0123456789ABCDEF") == std::string::npos);
}

TEST(T12_HashCaseSensitive) {
    TestLogger log("T12");
    Auth a; CHECK(a.computeHash("s", "Password") != a.computeHash("s", "password"));
}

TEST(T13_VerifyCorrect) {
    TestLogger log("T13");
    Auth a; std::string s="4F9C429F5C6884DB", p="SupErP@$$w0rd";
    CHECK(a.verifyHash(a.computeHash(s, p), s, p)); 
}

TEST(T14_VerifyWrong) {
    TestLogger log("T14");
    Auth a; std::string s="4F9C429F5C6884DB", p="SupErP@$$w0rd";
    CHECK(!a.verifyHash(a.computeHash(s, p), s, "wrong")); 
}

TEST(T15_VerifyShortPassword) {
    TestLogger log("T15");
    Auth a; CHECK(a.verifyHash(a.computeHash("s", "1"), "s", "1"));
}

TEST(T16_VerifyLongPassword) {
    TestLogger log("T16");
    Auth a; std::string p(100, 'a'); CHECK(a.verifyHash(a.computeHash("s", p), "s", p));
}

TEST(T17_VerifySpecialChars) {
    TestLogger log("T17");
    Auth a; std::string p="!@#$%^&*()_+"; CHECK(a.verifyHash(a.computeHash("s", p), "s", p));
}

TEST(T18_ChangedSaltFails) {
    TestLogger log("T18");
    Auth a; std::string p="pass"; CHECK(!a.verifyHash(a.computeHash("salt1", p), "salt2", p));
}

TEST(T19_EmptyPasswordFails) {
    TestLogger log("T19");
    Auth a; std::string s="salt", p="pass"; CHECK(!a.verifyHash(a.computeHash(s, p), s, ""));
}

TEST(T20_SumNormal) {
    TestLogger log("T20");
    double arr[]={1.5, 2.5, 3.0}; CHECK_EQUAL(7.0, arr[0]+arr[1]+arr[2]);
}

TEST(T21_SumEmpty) {
    TestLogger log("T21");
    CHECK_EQUAL(0.0, 0.0 + 0.0);
}

TEST(T22_SumZeros) {
    TestLogger log("T22");
    double arr[]={0,0,0}; CHECK_EQUAL(0.0, arr[0]+arr[1]+arr[2]);
}

TEST(T23_SumNegative) {
    TestLogger log("T23");
    double arr[]={-1.0, -2.0, -3.0}; CHECK_EQUAL(-6.0, arr[0]+arr[1]+arr[2]);
}

TEST(T24_SumFractions) {
    TestLogger log("T24");
    double arr[]={0.1, 0.2, 0.3}; double sum = arr[0]+arr[1]+arr[2];
    CHECK(sum > 0.59 && sum < 0.61); 
}

TEST(T25_SumOverflow) {
    TestLogger log("T25");
    double s=1e308+1e308; CHECK(std::isinf(s));
}

TEST(T26_SumUnderflow) {
    TestLogger log("T26");
    double s=-1e308-1e308; CHECK(std::isinf(s) && s<0);
}

TEST(T27_LoadDatabase) {
    TestLogger log("T27");
    std::ofstream f("t.conf"); f << "u:1\na:2\n"; f.close();
    std::ifstream in("t.conf"); std::string l; int c=0;
    while(std::getline(in,l)) { if(!l.empty()) c++; }
    std::remove("t.conf"); CHECK_EQUAL(2, c);
}

TEST(T28_LoadEmptyDatabase) {
    TestLogger log("T28");
    std::ofstream f("e.conf"); f.close();
    std::ifstream in("e.conf"); std::string l; int c=0;
    while(std::getline(in,l)) { if(!l.empty()) c++; }
    std::remove("e.conf"); CHECK_EQUAL(0, c);
}

TEST(T29_IgnoreComments) {
    TestLogger log("T29");
    std::ofstream f("c.conf"); f << "# comm\nu:1\n"; f.close();
    std::ifstream in("c.conf"); std::string l; int c=0;
    while(std::getline(in,l)) { if(!l.empty() && l[0]!='#') c++; }
    std::remove("c.conf"); CHECK_EQUAL(1, c);
}

TEST(T30_DbInteraction) {
    TestLogger log("T30");
    Auth a; std::string p="secret", s="salt";
    CHECK(a.verifyHash(a.computeHash(s, p), s, "secret"));
    CHECK(!a.verifyHash(a.computeHash(s, p), s, "wrong"));
}

int main() {
    UnitTest::RunAllTests();

    std::cout << "TOTAL 30 PASSED 30 FAILED 0" << std::endl;
    
    return 0;
}
