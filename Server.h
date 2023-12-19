#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <vector>
#include "GerUsers.h"
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include "GerThreads.h"
#include "CLResponse.h"
#include <mutex>
#include <chrono>
#include <semaphore.h>

#define HOST = '127.0.0.1';
#define PORT = 4000;

using namespace std;


class Server{
private:
    int svSocket;
    sockaddr_in serverAddr;
    User* root;
    GerUsers* users             = new GerUsers();
    GerThreads* threads         = new GerThreads();
    vector<User*> reqUsersConnections;
    vector<User*> reqUsersDisconnect;
    vector<Message> reqFollow;
    vector<Message> reqTweeats;
    int pointerPost = 0;
    std::mutex gate;
    sem_t sem_start;
    sem_t sem_req_connections;
    sem_t sem_follow;
    sem_t sem_tweet;

    bool flagServer = true;
    bool flagGerReqConnections = true;
    
    void header(string message, bool flagNewLine);
    void setSocket(int svSocket);
    bool createSocket();
    void addressConfig();
    bool bindSocket();
    void receiveMessages();
    void gerReqConnections();
    void gerReqDisconnect();
    void gerReqTweets();
    void gerReqFollow();
    int sendMessage(User* user, string command, string message);
    int sendMessage(User* user, Message stMessage);
    void disconnectUser(User* user);
    void sincronizarUsuário(User* user);

public:
    Server(/* args */);
    ~Server();
    void start();
};



#endif