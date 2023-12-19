#ifndef CLRESPONSE_H
#define CLRESPONSE_H

#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <chrono>
using namespace std;

struct Response{
    bool flag;
    string message;

    Response(bool flagSuccess, string message)
        : flag(flagSuccess), message(message) {}
};

struct Message{
    long long timestamp;
    char command[20];
    char user[30];
    char message[200];
};

class CLResponse{
private:
    /* data */
public:
    CLResponse(/* args */);
    ~CLResponse();
    
    long long getTimestamp();
    char* encMessage(string command, string user, string message);
    char* encMessage(Message stMessage);
    Message decMessage(char* buffer);
};

#endif
