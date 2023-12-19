#include "User.h"


User::User(int socket, string name) : address({}){
    this->timestamp = this->getTimestamp();
    this->socket    = socket;
    this->name      = name;
    this->flagConnection = false;
}

User::User(string name) : address({}){
    this->timestamp = this->getTimestamp();
    this->name = name;
}

long long User::getTimestamp(){
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();

    return milliseconds;
}

/**
 * [GET NAME]
 * 
 * A função retorna o nome do usuário
 * 
 * @return string
*/
string User::getName(){
    return this->name;
}

/**
 * [CLOSE SOCKET]
 * 
 * A função encerra o socket do cliente
*/
void User::closeSocket(){
    close(this->socket);
}

/**
 * [ADD FOLLOW]
 * 
 * A função adiciona um novo seguidor
 * na lista e retorna um Response com:
 * flag     : bool indicando que foi adicionado
 * message  : string informando a situação
 * 
 * @param User* user
 * @return Response 
*/
Response User::follow(User* user){
    bool flagFollowing = false;
    string message;

    if(!this->isFollowing(user) && this != user){
        int size = this->following.size();
        
        this->following.push_back(user);

        if(size < this->following.size()){
            message = "Voce esta seguindo o " + user->getName();
            flagFollowing = true;
        }else{
            message = "Falha ao seguir o " + user->getName();
        }
    }else{
        message = "Voce ja segue o usuario\n";
    }

    return Response(flagFollowing, message);
}

/**
 * [GET FOLLOWS]
 * 
 * A função retorna a lista quem o usuário segue
 * 
 * @return vector<User*>
*/
vector<User*> User::getFollowing(){
    return this->following;
}


/**
 * [getListFollowing]
 * 
 * A função retorna a lista de usuários que o usuário segue
 * 
*/
vector<User*> User::getListFollowing(){
    return this->following;
}

/**
 * [setListFollowing]
 * 
 * A função retorna a lista de usuários que o usuário segue
 * 
*/
void User::setListFollowing(vector<User*> listFollowing){
    this->following = listFollowing;
}


/**
 * [PRINT FOLLOWS]
 * 
 * A função printa todos os seguidores do usuários
*/
void User::printFollowers(){
    for(User* follow : this->getFollowing()){
        std::cout << follow->getName() << endl;
    }
}

void User::printFollowings(){
    cout << "\n\n--------" << endl;
    cout << this->getName() << " segue:" << endl;
    for(User* follow : this->getFollowing()){
        std::cout << "\t- " << follow->getName() << endl;
    }
}


/**
 * [UNFOLLOW]
 * 
 * A função remove o seguidor da lista, caso esteja nela e 
 * retorna um Response indicando:
 * flag     : bool indicando se sdeixou de seguir
 * message  : string indicando a situação
 * 
 * @param User* user
 * @return Response
*/
Response User::unfollow(User* user){
    bool flagRemove = false;
    string message;

    int lenFollows = this->following.size();
    int indexRemove = -1;

    // Pega o indice do seguidor que deve ser removido
    for(int i = 0; i < lenFollows; i++){
        if(this->following[i] == user){
            indexRemove = i;
        }
    }

    // Verifica se o seguidor existe na lista
    if(indexRemove > -1){
        this->following.erase(this->following.begin() + indexRemove);
        flagRemove = true;
        message = "Voce deixou de seguir o " + user->getName();
    }else{
        message = "Voce nao segue o usuario " + user->getName();
    }

    return Response(flagRemove, message);
}



/**
 * [IS FOLLOWER]
 * 
 * A função verifica se o usuário segue o
 * usuário informado
 * 
 * @param User* user
 * @return bool
 * 
*/
bool User::isFollowing(User* user){
    bool flagIsFollowing = false;

    for(User* u : this->getFollowing()){
        if(u == user){
            flagIsFollowing = true;
            break;
        }
    }

    return flagIsFollowing;
}


/**
 * [NEW POST]
 * 
 * A função envia o post para a fila de postagem
 * do servidor.
 * 
 * @param string message
 * @return bool
 * 
*/
Response User::newPost(string message){
    return Response(false, "Nao implementado");
}

bool User::compareSockAddrIn(const struct sockaddr_in& addr2) {
    // Comparação do endereço IP e da porta
    return (this->address.sin_family == addr2.sin_family) &&
           (this->address.sin_port == addr2.sin_port) &&
           (memcmp(&this->address.sin_addr, &addr2.sin_addr, sizeof(this->address.sin_addr)) == 0);
}

// Método para definir o endereço
void User::setAddress(const struct sockaddr_in &newAddress) {
    this->address.sin_family        = newAddress.sin_family;
    this->address.sin_addr.s_addr   = newAddress.sin_addr.s_addr;
    this->address.sin_port          = newAddress.sin_port;

}
