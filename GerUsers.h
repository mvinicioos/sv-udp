#ifndef GER_USERS_H
#define GER_USERS_H

#include <iostream>
#include <vector>
#include "User.h"
#include "CLResponse.h"

#define LIMIT_USERS 2

using namespace std;


class GerUsers{
private:
    vector<User*> users;
    
public:
    GerUsers(/* args */);
    ~GerUsers();

    // Verifica se o usuário não excedeu o limit de conexões
    Response checkLimitUsers(User* user);

    // Add uma nova conexão
    Response addNewConection(int socket, string name);
    
    // Add uma nova conexão
    Response addNewConection(User* user);

    // Remove um usuário da lista de conectados
    Response removeUser(User* user);

    // Retorna um vector com todos seguidores do usuário
    vector<User*> getFollowers(User* user);

    // Consome dois usuários: A e B onde o A passa a seguir o B
    Response follow(User* userA, User* userB);

    // Consome dois usuários: A e B onde o A deixa de seguir o B.
    Response unfollow(User* userA, User* userB);

    // Retorna o primeiro usuário encontrado com o nome de identificação
    User* getUser(string name);

    // Retorna um dispositivo em específico
    User* getUser(string name, const struct sockaddr_in& address);

    // Retorna um dispositivo em específico
    vector<User*> getAllUsers();

    long long getLastTimestamp(User* user);

    // Verifica se o usuário existe
    bool existUser(string name);

    void printAll(bool flagConnected);

    
};

#endif
