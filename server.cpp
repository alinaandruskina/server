#include "server.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

Server::Server() : server_socket(-1) {}
Server::~Server() { if (server_socket != -1) close(server_socket); }

bool Server::parseArgs(int argc, char** argv) {
    po::options_description desc("Параметры");
    desc.add_options()
        ("help,h", "Справка")
        ("port,p", po::value<int>(&params.port)->required(), "Порт")
        ("auth,a", po::value<std::string>(&params.auth_file)->required(), "Файл базы")
        ("log,l", po::value<std::string>(&params.log_file)->default_value("server.log"), "Файл лога");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help")) { std::cout << desc << "\n"; return false; }
        po::notify(vm);
    } catch (std::exception& e) {
        std::cerr << "Ошибка параметров: " << e.what() << "\n";
        return false;
    }
    return true;
}

void Server::loadDatabase() {
    std::ifstream file(params.auth_file);
    if (!file.is_open()) throw std::runtime_error("Не могу открыть файл базы!");
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            db[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
}

std::string Server::recvString(int client_sock) {
    char buf[1024] = {0};
    int bytes = recv(client_sock, buf, sizeof(buf) - 1, 0);
    if (bytes <= 0) throw std::runtime_error("Клиент отвалился");
    return std::string(buf);
}

void Server::sendString(int client_sock, const std::string& msg) {
    send(client_sock, msg.c_str(), msg.length(), 0);
}

double Server::calculateSum(double* arr, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
        if (std::isinf(sum)) {
            return sum > 0 ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
        }
    }
    return sum;
}

void Server::start() {
    loadDatabase();
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(params.port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (sockaddr*)&addr, sizeof(addr)) < 0) throw std::runtime_error("Ошибка bind");
    if (listen(server_socket, 5) < 0) throw std::runtime_error("Ошибка listen");

    std::cout << "Сервер запущен на порту " << params.port << std::endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) continue;

        try {
            std::string login = recvString(client_socket);
            if (db.find(login) == db.end()) {
                sendString(client_socket, "ERR");
                close(client_socket); continue;
            }

            std::string salt = auth.generateSalt();
            sendString(client_socket, salt);
            std::string client_hash = recvString(client_socket);
            
            if (!auth.verifyHash(client_hash, salt, db[login])) {
                sendString(client_socket, "ERR");
                close(client_socket); continue;
            }

            sendString(client_socket, "OK");

            uint32_t N;
            recv(client_socket, &N, sizeof(N), 0);
            for (uint32_t i = 0; i < N; i++) {
                uint32_t S;
                recv(client_socket, &S, sizeof(S), 0);
                double* vec = new double[S];
                recv(client_socket, vec, S * sizeof(double), 0);
                double result = calculateSum(vec, S);
                delete[] vec;
                send(client_socket, &result, sizeof(result), 0);
            }
        } catch (std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
        close(client_socket);
    }
}
