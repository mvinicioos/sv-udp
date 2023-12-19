#include "GerUsers.h"

GerUsers::GerUsers(){
}

GerUsers::~GerUsers(){
}

vector<User*> GerUsers::getAllUsers(){
    return this->users;
}

/**
 * [CHECK LIMIT USERS]
 * 
 * A função valida a conexão do usuário, retornando um Response,
 * response.flag true, usuário autorizado.
 * 
 * @param User*
 * @return Response
*/
Response GerUsers::checkLimitUsers(User* user){
    bool flagAutorized  = false;
    string message      = "Limite de conexoes excedido";
    int numConnections  = 0;
    int limitUsers      = LIMIT_USERS;
    
    if(user->getName() == "root" && this->users.size() == 0){
        flagAutorized = true;
        message = "Usuário root iniciado";
    }else{
        User* oldUser = this->getUser(user->getName(), user->address);

        if(oldUser == NULL){
            for(User* u : this->users){
                //Compara nome de usuário e flag de conexão para verificar qusntos usuários existem
                if(strcmp(u->getName().c_str(), user->getName().c_str()) == 0 && u->flagConnection){
                    numConnections++;
                }
            }
            //
            if(numConnections < LIMIT_USERS){
                flagAutorized = true;
                message = "new-user";
            }
        }else if(oldUser->flagConnection == false){
            cout << "> Retomando conexão com o usuário" << endl;
            message = "connection-retomed";

        }
    }

    return Response(flagAutorized, message);
}

/**
 * [ADD NEW USER]
 * 
 * A função verifica se o usuário não excedeu o limite de conexões e 
 * então inclui uma nova conexão do usuário.
 * 
 * @param user
 * @return Response
*/
Response GerUsers::addNewConection(int socket, string name){
    User* user = new User(socket, name);

    Response resp = this->checkLimitUsers(user);

    if(resp.flag){
        user->flagConnection = true;
        this->users.push_back(user);
        resp.message = user->getName() + " conectado";
        
    }

    return resp;
}

/**
 * ADD NEW CONNECTION
 * 
 * Alternativa para a inserção de usuário
 * 
 * @param User
 * @return Response
*/
Response GerUsers::addNewConection(User* user){
    Response resp = this->checkLimitUsers(user);

    if(resp.flag){
        user->flagConnection = true;
        this->users.push_back(user);
        resp.message = user->getName() + " conectado";
    }

    return resp;
}


/**
 * [REMOVE USER]
 * 
 * A função remove um usuário da lista de usuários
 * 
 * @param User*
 * @return Response
*/
Response GerUsers::removeUser(User* user){
    bool flagUserRemoved = false;
    string message = user->getName() + " nao esta conectado";
    int index = -1;

    // Percorre a lista de usuário para verificar se o usuário está conectado
    for(int i = 0; i < this->users.size(); i++){        
        // Salva a posição do usuário
        if(user == this->users[i]){            
            index = i;
        }
    }

    // Com o index acima de -1, indica que o usuário foi encontrado, logo, o remove.
    if(index >= 0){
        flagUserRemoved = true;
        message = user->getName() + " desconectado";
        this->users.erase(this->users.begin() + index);
    }

    return Response(flagUserRemoved, message);
    
}




/**
 * [GET FOLLOWERS]
 * 
 * A função recebe um usuário e devolve uma lista
 * de usuários que o seguem
 * 
 * @param User*
 * @return vector<User*>
*/
vector<User*> GerUsers::getFollowers(User* user){
    vector<User*> listFollowers;

    // Percorrendo a lista de usuários
    for(User* u : this->users){
        // Testa se o usuário não é o próprio informado e se é seguido por esse
        if(u != user && u->isFollowing(user)){
            bool flagInside = false;

            for(User* f : listFollowers){
                if(u == f){
                    flagInside = true;
                    break;
                }
            }

            if(!flagInside){
                // Então u segue user
                listFollowers.push_back(u);
            }
            
        }
    }

    return listFollowers;
}


/**
 * [FOLLOW]
 * 
 * A função consome dois usuários e seta o usuário A 
 * a seguir o usuário B.
 * 
 * @param User* userA
 * @param User* userB
 * 
 * @return Response
*/
Response GerUsers::follow(User* userA, User* userB){
    Response resp = userA->follow(userB);

    return resp;
}


/**
 * [UNFOLLOW]
 * 
 * A função consome dois usuários, e o UserA deixa de seuir o UserB
 * 
 * @param User* userA
 * @param User* userB
 * @return Response
 * 
*/
Response GerUsers::unfollow(User* userA, User* userB){
    Response resp = userA->unfollow(userB);

    return resp;
}

/**
 * [GET USER]
 * 
 * A função recebe o nome do usuário e seu endereço vindo
 * e reconstruído pelo recvfrom, a função retorna qual usuário
 * se trata.
*/
User* GerUsers::getUser(string name, const struct sockaddr_in& address){
    User* respUser = NULL;

    for(User* user : this->users){
        if(strcmp(user->getName().c_str(), name.c_str()) == 0 && user->compareSockAddrIn(address)){
            respUser = user;            
        }
    }

    return respUser;
}


/**
 * [GET USER]
 * 
 * A função recebe o nome do usuário e retorna o primeiro usuário encontrado
 * 
 * @param string
*/
User* GerUsers::getUser(string name){
    User* respUser;

    for(User* user : this->users){
        if(strcmp(user->getName().c_str(), name.c_str()) == 0){
            respUser = user;            
        }
    }

    return respUser;
}




/**
 * [EXISTS USER]
 * 
 * A função verifica se o usuário 'name' existe entre os conectados ou desconectados;
 * @param string
*/
bool GerUsers::existUser(string name){
    bool flagExists = false;

    for(User* u : this->users){
        if(u->getName() == name){
            flagExists = true;
            break;
        }
    }

    return flagExists;
}



void GerUsers::printAll(bool flagConnected){
    
    cout << "> Total: " << this->users.size() << endl;

    for (User* user : this->users){
        if(flagConnected){
            if(user->flagConnection){
                cout << "\t- " << user->getName() << " em " << user->timestamp << endl;    
            }
        }else{
            cout << "\t- " << user->getName() << endl;
        }        
    }
}

long long GerUsers::getLastTimestamp(User* user){
    long long timestamp = -1;

    for(User* u : user->getFollowing()){
        if(!u->flagConnection){
            if(timestamp < u->timestamp){
                timestamp = u->timestamp; 
            }
        }
    }

    return timestamp;
}

