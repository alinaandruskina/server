#pragma once
#include <string>
#include <map>
#include <boost/program_options.hpp>
#include "auth.h"

namespace po = boost::program_options;

struct Params {
    int port;
    std::string auth_file;
    std::string log_file;
};

class Server {
private:
    Params params;
    Auth auth;
    std::map<std::string, std::string> db;
    int server_socket;
    void loadDatabase();
    std::string recvString(int client_sock);
    void sendString(int client_sock, const std::string& msg);
    double calculateSum(double* arr, int size);
public:
    Server();
    ~Server();
    bool parseArgs(int argc, char** argv);
    void start();
};
