#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include "CLResponse.h"
#include <unistd.h>
#include <chrono>


using namespace std;

class User{
private:
    int socket;
    string name;
    vector<User*> following; 
    

public:
    struct sockaddr_in address;
    bool flagConnection;
    long long timestamp;
    User(string name);
    User(int socket, string name);
    
    ~User();
    long long getTimestamp();
    
    // Retorna o nome do usuário
    string getName();
    
    bool compareSockAddrIn(const struct sockaddr_in& addr);


    // Adiciona um seguidor à lista do usuário
    Response follow(User* follow);

    // Retorna uma lista de seguidores
    vector<User*> getFollowing();
    
    // Seta a lista que o usuário segue
    void setListFollowing(vector<User*> listusers);

    // Retorna a lista dos usuários que o usuário segue
    vector<User*> getListFollowing();


    // Imprime a lista de seguidores
    void printFollowers();

    // Imprime os usuários que está seguindo
    void printFollowings();

    // Remove um seguidor
    Response unfollow(User* follow);

    // Dado um usuário, verifica se é seguidor
    bool isFollowing(User* user);

    // Realiza uma postagem
    Response newPost(string message);

    void setAddress(const struct sockaddr_in &newAddress);

    void closeSocket();
    

};

#endif