#include "server.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

Server* global_server = nullptr;

void signalHandler(int signum) {
    (void)signum;
    if (global_server) global_server->stop();
}

Server::Server() : server_socket(-1), isRunning(true), isOverflow(false) {
    global_server = this;
}

Server::~Server() { 
    if (server_socket != -1) close(server_socket); 
    if (log_file.is_open()) log_file.close();
}

void Server::writeLog(const std::string& msg) {
    std::cout << msg << std::endl;
    if (log_file.is_open()) {
        log_file << msg << std::endl;
        log_file.flush();
    }
}

void Server::printUsage() {
    std::cout << "Сервер vcalc - серверная программа для клиент-серверной системы обработки данных\n\n";
    std::cout << "Использование: vcalc_server [options]\n\n";
    std::cout << "Параметры:\n";
    std::cout << "  -h [ --help ]         Вывод справки\n";
    std::cout << "  -d [ --db-file ] arg  Имя файла с базой клиентов (по умолчанию ./vcalc.conf)\n";
    std::cout << "  -l [ --log-file ] arg Имя файла с журналом работы (по умолчанию ./vcalc.log)\n";
    std::cout << "  -p [ --port ] arg     Номер порта сервера (по умолчанию 33333)\n";
}

bool Server::parseArgs(int argc, char** argv) {
    params.port = 33333; 
    params.auth_file = "vcalc.conf";
    params.log_file = "vcalc.log";

    if (argc == 1) {
        printUsage();
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return false;
        } 
        else if (arg == "-d" || arg == "--db-file") {
            if (i + 1 < argc) params.auth_file = argv[++i];
        } 
        else if (arg == "-l" || arg == "--log-file") {
            if (i + 1 < argc) params.log_file = argv[++i];
        } 
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) params.port = std::stoi(argv[++i]);
        }
        else if (arg == "--run") continue;
        else if (arg == "-T" || arg == "-H" || arg == "-S") i++; // Для клиента препода
    }
    return true;
}

void Server::createDefaultDatabase() {
    std::ofstream file(params.auth_file);
    if (file.is_open()) {
        file << "user:P@ssW0rd\n";
        file << "ibst128:SupErP@$$w0rd\n";
        file.close();
        std::cout << "log: Создан файл базы по умолчанию: " << params.auth_file << std::endl;
    }
}

void Server::loadDatabase() {
    std::ifstream file(params.auth_file);
    if (!file.is_open()) {
        createDefaultDatabase();
        file.open(params.auth_file);
    }
    if (!file.is_open()) throw std::runtime_error("Не могу открыть или создать файл базы!");

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
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
    isOverflow = false;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
        if (std::isinf(sum)) {
            isOverflow = true;
            return sum > 0 ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
        }
    }
    return sum;
}

void Server::stop() {
    if (!isRunning) return; 
    isRunning = false;
    writeLog("log: Server stopped by signal 2");
    if (server_socket != -1) { close(server_socket); server_socket = -1; }
    if (log_file.is_open()) log_file.close();
    exit(0); 
}

void Server::start() {
    signal(SIGINT, signalHandler); 

    log_file.open(params.log_file, std::ios::app);
    writeLog("log: Select variant double:SHA1:server");
    writeLog("log: digest size: 20");
    
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

    while (isRunning) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) { if (!isRunning) break; continue; }

        try {
            std::string login = recvString(client_socket);
            writeLog("log: received USERNAME " + login);
            
            if (db.find(login) == db.end()) {
                sendString(client_socket, "ERR");
                writeLog("log: username fail");
                close(client_socket); 
                continue;
            }
            writeLog("log: username ok");

            std::string salt = auth.generateSalt();
            sendString(client_socket, salt);
            writeLog("log: sending SALT " + salt);
            
            std::string client_hash = recvString(client_socket);
            writeLog("log: received MESSAGE " + client_hash);
            
            if (!auth.verifyHash(client_hash, salt, db[login])) {
                sendString(client_socket, "ERR");
                writeLog("log: auth fail");
                close(client_socket); 
                continue;
            }

            sendString(client_socket, "OK");
            writeLog("log: auth success, sending OK");
            writeLog("log: Start calculate with type d");

            uint32_t N;
            if (recv(client_socket, &N, sizeof(N), 0) <= 0) throw std::runtime_error("Ошибка чтения N");
            writeLog("log: Numbers of vectors " + std::to_string(N));
            
            for (uint32_t i = 0; i < N; i++) {
                uint32_t S;
                if (recv(client_socket, &S, sizeof(S), 0) <= 0) throw std::runtime_error("Ошибка чтения S");
                
                writeLog("log: Vector " + std::to_string(i) + " size " + std::to_string(S));
                
                double* vec = new double[S];
                if (recv(client_socket, vec, S * sizeof(double), 0) <= 0) {
                    delete[] vec; 
                    throw std::runtime_error("Ошибка чтения вектора");
                }
                
                std::string vec_str = "log: Vector values: ";
                for(uint32_t j=0; j<S; j++) vec_str += std::to_string(vec[j]) + " ";
                writeLog(vec_str);

                double result = calculateSum(vec, S);
                delete[] vec;
                
                writeLog("log: Sended vector summ " + std::to_string(result));
                send(client_socket, &result, sizeof(result), 0);
            }
        } catch (std::exception& e) {
            writeLog(std::string("log: Error: ") + e.what());
        }
        close(client_socket);
        writeLog("log: Connection close");
        writeLog("======================");
    }
}
