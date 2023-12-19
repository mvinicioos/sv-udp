#include "Client.h"

using namespace std;

Client::Client(){
    
}

/**
 * [SET SOCKET]
 * 
 * A função seta o socket do servidor
 * 
 * @param int svSocket
 * 
*/
void Client::setSocket(int cliSocket){
    this->cliSocket = cliSocket;
}


/**
 * [CREATE SOCKET]
 * 
 * A função cria o socket, seta e retorna um bool
 * 
 * @return bool
*/
bool Client::createSocket(){
    bool flagCreateSocket = true;

    int cliSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if(cliSocket == -1){
        flagCreateSocket = true;
    }else{
        this->setSocket(cliSocket);
    }

    return flagCreateSocket;
}

/**
 * [ADDRESS CONFIG]
 * 
 * A função configura o endereço do servidor
*/
void Client::addressSVConfig(string ipAddress, int port){
    memset((char*)&(this->svAddr), 0, sizeof(this->svAddr));
    this->svAddr.sin_family         = AF_INET;
    this->svAddr.sin_port           = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &(this->svAddr).sin_addr);
}

/**
 * [START]
 * 
 * A função centraliza as ações do servidor
*/
void Client::start(string user, string ipAddress, int port){
    
    if(this->createSocket()){
        sem_init(&this->sem_conexx, 0, 0);

        string buffer;
        // Setando usuário
        cout << "id_ser: " << user << endl;
        this->user = new User(this->cliSocket, user);
        this->addressSVConfig(ipAddress, port);
        this->flagRunning = true;        
        cout << "> Inicializando threads..." << endl;
        //--------------------------------------------------- THREADS
        //Criando a thread para receber mensagens
        thread* thReceiveMessages = new thread([this]{
            cout << "> Gerenciador de mensagens inicializado" << endl;
            this->receiveMessages();
        });

        this->threads->addThread(thReceiveMessages, this->user, "sys-receive-messages");
        //---------------------------------------------------
        std::this_thread::sleep_for(std::chrono::seconds(3));

        int sentBytes = this->sendMessage("req-connect", this->user->getName(), "req-connect");
        
        if(sentBytes > 0){
            cout << "> Conectando...";
            sem_wait(&this->sem_conexx);
            if(this->user->flagConnection){
                cout << " [Conectado]" << endl;
            }          
        }else{
            cout << "Falha ao enviar mensagem" << endl;
            //this->flagRunning = false;
            //close(this->cliSocket);
        }

        while(this->user->flagConnection && this->flagRunning){
            cout << "Send " << endl << "> ";
            getline(cin, buffer);

            if(buffer.size() > 0){
                Message stMessage = this->commandLine(buffer, this->user->getName());

                this->sendMessage(stMessage.command, stMessage.user, stMessage.message);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        };
    }
}

int Client::sendMessage(string command, string user, string message){
    int sentBytes = 0;

    if(this->flagRunning){
        CLResponse* cl = new CLResponse();
        char* buffer = cl->encMessage(command, user, message);
        
        sentBytes = sendto(this->cliSocket, buffer, sizeof(Message), 0, (struct sockaddr*)&this->svAddr, sizeof(this->svAddr));
    }

    return sentBytes;
}

int Client::receiveMessages(){
    struct Message stMessage;
    char buffer[sizeof(struct Message)];    
    CLResponse* cl = new CLResponse();
    socklen_t svAddrLen = sizeof(this->svAddr);
    
    while(this->flagRunning){
        
        int bytesRec = recvfrom(this->cliSocket, buffer, sizeof(struct Message), 0, (struct sockaddr *)&this->svAddr, &svAddrLen);

        stMessage = cl->decMessage(buffer);
        
        if(strcmp(stMessage.command,        "sys-connected") == 0){
            this->user->flagConnection = true;
            //cout << stMessage.message << endl;
            sem_post(&this->sem_conexx);            
        }else if(strcmp(stMessage.command, "disconnect") == 0){
            this->user->flagConnection = false;
            this->flagRunning = false;
            this->user->closeSocket();
            cout << endl << stMessage.message << endl;
            sem_post(&this->sem_conexx);            
        }else if(strcmp(stMessage.command,  "return-follow") == 0){
            cout << stMessage.message << endl;
        }else if(strcmp(stMessage.command,  "return-follow") == 0){
            cout << "\n" << stMessage.message << endl << endl; 
        }else if(strcmp(stMessage.command,  "send") == 0){
            cout << "-------------- ";
            this->printTimestamp(stMessage.timestamp);
            cout << "Usuário " << stMessage.user << " diz:";
            cout << endl << "- " << stMessage.message << endl << endl;
        }
        
    }
    cout << "> Gerenciador de mensagens finalizado" << endl;

    return 0;
}


/**
 * [COMAND LINE]
 * 
 * A função recebe a linha de comando e separa o que é comando
 * e o que é mensagem.
 * 
 * @param string
 * @param string 
 * @return Message
*/
Message Client::commandLine(string line, string user){
    Message stMessage;
    size_t pos = line.find(' ');
    string command;
    string message;
    
    command = line.substr(0, pos);
    cout << "> Formando command line " << endl;

    if(pos >= 4 && pos < 15){
        message = (pos == std::string::npos) ? "" : line.substr(pos + 1);
    }

    cout << command << endl;
    cout << message << endl;

    if(!message.empty() && message[0] == '@'){
        message = message.substr(1);
    }

    //Montando struct
    strncpy(stMessage.command,  command.c_str(),    sizeof(stMessage.command)   - 1);
    strncpy(stMessage.user,     user.c_str(),       sizeof(stMessage.user)      - 1);
    strncpy(stMessage.message,  message.c_str(),    sizeof(stMessage.message)   - 1);

    return stMessage;
}

int main(int argc, char const *argv[]){    
    if(argc != 4){
        cout << "> Argumentos inválidos!" << endl;
        return 0;
    }
    
    // Separando os parâmetros de entrada
    string user         = argv[1];
    string ipAddress    = argv[2];
    int port            = std::atoi(argv[3]);

    Client* cli = new Client();   

    cli->start(user, ipAddress, port);
    
    cli->threads->joinAll();
    
    cout << "Cliente finalizado" << endl;
    return 0;
}


void Client::printTimestamp(long long timestamp){
    // Converter o timestamp para time_t
    // Convertendo o timestamp de volta para um objeto de tempo
    std::chrono::milliseconds timestampDuration(timestamp);
    std::chrono::system_clock::time_point time = std::chrono::time_point<std::chrono::system_clock>(timestampDuration);

    // Convertendo para um formato de data e hora legível
    std::time_t convertedTime = std::chrono::system_clock::to_time_t(time);
    std::cout << std::ctime(&convertedTime);
}