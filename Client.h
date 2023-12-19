#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <vector>

#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <algorithm>
#include "User.h"
#include "CLResponse.h"
#include "GerThreads.h"
#include "thread"
#include <mutex>
#include <semaphore.h>
#include <chrono>
#include <ctime>

using namespace std;

class Client{
private:
    int cliSocket;
    sockaddr_in svAddr;
    User* user;
    sem_t sem_conexx;
    bool flagRunning = false;

    bool getParams(int argc, char const *argv[]);
    void setSocket(int svSocket);
    bool createSocket();
    void addressSVConfig(string ipAddress, int port);

    Response waitResponseForConnection();
    void connect(string user);
    //bool sendMessage(const char* message);

public:
    Client();
    ~Client();
    GerThreads* threads = new GerThreads();
     void start(string user, string ipAddress, int port);
     int sendMessage(string command, string user, string message);
     int receiveMessages();
     Message commandLine(string line, string user);
     void printTimestamp(long long timestamp);
     
};

#endif