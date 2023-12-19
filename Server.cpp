#include "Server.h"


Server::Server(/* args */){
}

Server::~Server(){
}

/**
 * [SET SOCKET]
 * 
 * A função seta o socket do servidor
 * 
 * @param int svSocket
 * 
*/
void Server::setSocket(int svSocket){
    this->svSocket = svSocket;
}

void Server::sincronizarUsuário(User* user){
    cout << "> Sincronizando..." << endl;
    long long timestamp = this->users->getLastTimestamp(user);
    cout << timestamp << endl;
    this->gate.lock();
    if(timestamp > -1){
        cout << "\n\n> Sincronizando o usuário" << endl;
        for(Message tweeat : this->reqTweeats){
            if(strcmp(tweeat.user, user->getName().c_str()) == 0 && tweeat.timestamp > user->timestamp){
                this->sendMessage(user, tweeat);
            }
        }
    }
    this->gate.unlock();
}
/**
 * [CREATE SOCKET]
 * 
 * A função cria o socket, seta e retorna um bool
 * 
 * @return bool
*/
bool Server::createSocket(){
    cout << "> Criando socket.." << endl;
    bool flagCreateSocket = true;

    int svSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if(svSocket == -1){
        flagCreateSocket = false;
    }else{
        this->setSocket(svSocket);
    }

    return flagCreateSocket;
}


/**
 * [ADDRESS CONFIG]
 * 
 * A função configura o endereço do servidor
*/
void Server::addressConfig(){
    cout << "> Configurkando endereço" << endl;
    memset((char*)&(this->serverAddr), 0, sizeof(this->serverAddr));
    this->serverAddr.sin_family         = AF_INET;
    this->serverAddr.sin_addr.s_addr    = htonl(INADDR_ANY);
    this->serverAddr.sin_port           = htons(4000);
}


/**
 * [BIND SOCKET]
 * 
 * A função vincula o endereço ao socket
 * 
 * @return bool
*/
bool Server::bindSocket(){
    cout << "> Vinculando endereço ao socket" << endl;
    bool flagBind = true;
    
    if(bind(this->svSocket, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) == -1){
        flagBind = false;
    }

    return flagBind;
}

/**
 * [SEND MESSAGE]
 * 
 * A função envia uma mensagem ao user*
 * 
 * @param User*
 * @param string
 * @param string
*/
int Server::sendMessage(User* user, string command, string message){
    CLResponse* cl = new CLResponse();
    
    char* buffer = cl->encMessage(command, user->getName(), message);
    
    int sentBytes = sendto(this->svSocket, buffer, sizeof(Message), 0, (struct sockaddr*)&(user->address), sizeof(user->address));

    return sentBytes;
}


/**
 * [SEND MESSAGE]
 * 
 * Sobrecarga da função SEND MESSAGE, que recebe apenas o usuários
 * destinatário.
 * 
 * @param User
 * @param string
 * @param string
*/
int Server::sendMessage(User* user, Message stMessage){
    CLResponse* cl = new CLResponse();
    
    char* buffer = cl->encMessage(stMessage);
    
    int sentBytes = sendto(this->svSocket, buffer, sizeof(Message), 0, (struct sockaddr*)&(user->address), sizeof(user->address));

    return sentBytes;
}

/**
 * [GER REQ CONNECTIONS] [THREAD]
 * 
 * A função gerencia as requisições de conexões
*/
void Server::gerReqConnections(){
    int maxRetries = 3;

    while(this->flagGerReqConnections && this->flagServer){        
        if(!this->reqUsersConnections.empty()){
            sem_wait(&this->sem_req_connections);

            User* user = this->reqUsersConnections.at(0);
            this->reqUsersConnections.erase(this->reqUsersConnections.begin());
            
            bool flagSharedList = this->users->existUser(user->getName());

            //---------------------------------------------- TEXT
            
            Response resp = this->users->addNewConection(user);
            
            if(resp.flag){
                this->header("\tUSUÁRIOS ONLINE", true);
                this->users->printAll(true);
                //Em caso de não ser o primeiro dispositivo a seconectar com mesmo login a lista é compartilhada
                if(flagSharedList){
                    cout << "> Compartilhando lista de seguidores.." << endl;
                    User* fuser = this->users->getUser(user->getName());
                    user->setListFollowing(fuser->getListFollowing());
                }
                
                this->sincronizarUsuário(user);
                this->sendMessage(user, "sys-connected", "Bem-vindo ao servidor");
            }else{
                this->sendMessage(user, "disconnect", "Limite de conexões excedido");
                cout << "Conexão recusada" << endl;
            }
        }
        
    }
}


/**
 * [GER REQ FOLLOW] [THREAD]
 * 
 * A função gerencias as requisições de de usuários
 * para seguir outros.
*/
void Server::gerReqFollow(){

    while(this->flagServer){
        sem_wait(&this->sem_follow);
        if(!this->reqFollow.empty()){
            string messageA, messageB;
            //Pega o primeiro elemento da lista e o remove logo após
            Message stMessage = this->reqFollow.at(0);
            this->reqFollow.erase(this->reqFollow.begin());
            //Pega o usuário requisitante
            User* userA = this->users->getUser(stMessage.user);
            
            //Pega o usuário requisitado
            User* userB = this->users->getUser(stMessage.message);

            if(strcmp(stMessage.command, "follow") == 0){
                //Segue
                Response resp = userA->follow(userB);
                messageA = "> SV: Você está seguindo o " + userB->getName();
                messageB = "> SV: " + userA->getName() + " está seguindo você";

                //Pega todos dispositivos conectados do userA
                if(resp.flag){
                    cout << "> Disparando mensagens" << endl;
                    for(User* u : this->users->getAllUsers()){
                        if(strcmp(u->getName().c_str(), userA->getName().c_str()) == 0 && u->flagConnection){
                            this->sendMessage(u, "return-follow", messageA);
                        }

                        if(strcmp(u->getName().c_str(), userB->getName().c_str()) == 0 && u->flagConnection){
                            this->sendMessage(u, "return-follow", messageB);
                        }
                    }
                }
            }else if(strcmp(stMessage.command, "unfollow") == 0){
                Response resp = userA->unfollow(userB);
                //Pega todos dispositivos conectados do userA
                cout << "> Disparando mensagens" << endl;
                for(User* u : this->users->getAllUsers()){
                    if(strcmp(u->getName().c_str(), userA->getName().c_str()) == 0 && u->flagConnection){
                        this->sendMessage(u, "return-follow", "Você deixou de seguir o " + userB->getName());
                    }
                }    
            }
            userA->printFollowings();            
        }
    }
}

/**
 * [GER REQ TWEETS]
 * A função gerencia as requisições de tweets, analisando 
 * uma solicitação, verificando o autor e distribuindo a mensagem
 * para todos que o seguem.
 * 
 * 
*/
void Server::gerReqTweets(){
    
    while(this->flagServer){
        if(!this->reqTweeats.empty()){
            sem_wait(&this->sem_tweet);
            this->gate.lock();

            //Pegando a primeira requisição de post da fila
            Message stMessage = this->reqTweeats.at(this->pointerPost);            

            //Pegando usuário responsável
            User* user = this->users->getUser(stMessage.user);
            
            //Pegando os seguidores desse usuário
            vector<User*> followers = this->users->getFollowers(user);
            
            //Dispara io post para todos usuários que seguem o usuário
            for(User* u : followers){
                if(u->flagConnection){
                    this->sendMessage(u, stMessage);
                }
            }

            this->pointerPost++;
            this->gate.unlock();
        }

    }
}

void Server::header(string message, bool flagNewLine){
    if(flagNewLine){
        cout << "\n\n\n----------------------------------------" << endl;
    }else{
        cout << "----------------------------------------" << endl;
    }
    
    cout << message << endl;
    cout << "----------------------------------------" << endl;
}

/**
 * RECEIVE MESSAGES
 * 
 * A função fica ouvindo mensagens e trata as mensagens
*/
void Server::receiveMessages(){
    struct Message stMessage;
    char buffer[sizeof(struct Message)];
    sockaddr_in clientAddr;
    socklen_t cliAddrLen = sizeof(clientAddr);
    
    memset((char*)&(clientAddr), 0, sizeof(clientAddr));
    memset((char*)&(stMessage), 0, sizeof(stMessage));
    CLResponse* cl = new CLResponse();
    std::mutex gate;
    sem_post(&this->sem_start);
    this->header("\tSERVIDOR ONLINE", true);        
    while(this->flagServer){
        //------------------------------------------------ RECEBENDO MENSAGEM
        int bytesRec = recvfrom(this->svSocket, buffer, sizeof(struct Message), 0, (struct sockaddr *)&clientAddr, &cliAddrLen);
        
        //--------------------------------------------------- TRATANDO MENSAGEM
        if(bytesRec > 0){
            //Montagem do usuário para manipulação
            stMessage = cl->decMessage(buffer);
            User* user = new User(stMessage.user);
            user->setAddress(clientAddr);

            // Cria um novo usuário e coloca na lista de requisições de conexões
            if(strcmp(stMessage.command, "req-connect") == 0){
                //--------------------------------------------------- TEXT                
                cout << "\n# REQUISIÇÃO DE CONEXÃO" << endl;
                cout << "\t> Solicitante: " << stMessage.user << endl;
                
                this->reqUsersConnections.push_back(user);    
                sem_post(&this->sem_req_connections);

            }else if(strcmp(stMessage.command, "exit") == 0){
                User* user = this->users->getUser(stMessage.user, clientAddr);
                if(user != NULL){
                    user->flagConnection = false;
                    this->sendMessage(user, "disconnect", "");
                }
            }else if(strcmp(stMessage.command, "follow") == 0 || strcmp(stMessage.command, "unfollow") == 0){
                //Valida se o usuário existe
                if(this->users->existUser(stMessage.message)){
                    // Coloca na lista de requisição para seguir
                    this->reqFollow.push_back(stMessage);
                    sem_post(&this->sem_follow);
                }               
                
            }else if(strcmp(stMessage.command, "send") == 0){
                cout << "\n# REQUISIÇÃO DE POST" << endl;
                cout << "\t> " << stMessage.user << " requisitando post" << endl;
                cout << "\t> " << stMessage.message << endl;
                this->reqTweeats.push_back(stMessage);
                sem_post(&this->sem_tweet);
            }else{
                this->sendMessage(user, "error-message", "Comando inválido!");
            }
        }
    }
}

/**
 * [START]
 * 
 * A função centraliza as ações do servidor
*/
void Server::start(){
    this->header("\tINICIALIZAÇÕES", false);
    if(this->createSocket()){
        this->addressConfig();
        if(this->bindSocket()){
            cout << "> Servidor inicializado" << endl;
            cout << "> Inicializando usuário root..." << endl;
            // Inicialização do usuário root
            this->root = new User(this->svSocket, "root");
            Response resp = this->users->addNewConection(this->svSocket, "root");
            if(resp.flag){
                cout << "> Inicializando semáforos..." << endl;
                sem_init(&this->sem_start, 0, 0);
                sem_init(&this->sem_req_connections, 0, 0);
                sem_init(&this->sem_follow, 0, 0);
                sem_init(&this->sem_tweet, 0, 0);


                cout << "> Inicializando threads..." << endl;                

                //-------------------------------------------------------------------- THREADS
                //Criando thread responsável por gerenciar as requisições de conexões
                std::thread* thGerReqConnections = new thread([this]{
                    cout << "> Criando thread para gerenciar requisições de conexões" << endl;
                    this->gerReqConnections();
                });
                
                std::thread* thGerReqFollow = new thread([this]{
                    cout << "> Criando thread para seguidores" << endl;
                    this->gerReqFollow();
                });
                
                std::thread* thGerTweets = new thread([this]{
                    cout << "> Criando a thread para gerenciar os tweets";
                    this->gerReqTweets();
                });
                
                //Criando a thread responsável por receber mensagem dos usuários
                std::thread* thReceiveMessages = new thread([this]{
                    cout << "> Criando thread para gerenciar mensagens" << endl;
                    this->receiveMessages();
                });

                sem_wait(&this->sem_start);
                this->threads->addThread(thGerReqConnections, this->root, "sys-ger-connections");
                this->threads->addThread(thGerReqFollow, this->root, "sys-ger-follows");
                this->threads->addThread(thGerTweets, this->root, "sys-ger-tweats");
                this->threads->addThread(thReceiveMessages, this->root, "sys-receive-messages");

                //--------------------------------------------------------------------
                
            }
            
        }
        this->threads->joinAll();
    }
}

int main(int argc, char const *argv[]){
    
    Server* sv = new Server();
    sv->start();
    
    return 0;
}
