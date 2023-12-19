#ifndef GER_THREADS_H
#define GER_THREADS_H

#include <iostream>
#include <thread>
#include "User.h"
#include <string>

using namespace std;

// Criando uma estrutura para armazenar a thread e informações relevantes
struct STDescThread{
    thread* th;
    User* user;
    string desc;
    bool exec;

    STDescThread(thread* t, User* u, string d)
        : th(t), user(u), desc(d), exec(true){}
};


class GerThreads{
private:
    vector<STDescThread> threads;

public:
    GerThreads(/* args */);
    ~GerThreads();
    bool addThread(thread* th, User* user, string desc);
    void joinAll();
};



#endif
