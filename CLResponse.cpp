#include "CLResponse.h"

CLResponse::CLResponse(/* args */){
}

CLResponse::~CLResponse(){
}

long long CLResponse::getTimestamp(){
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();

    return milliseconds;
}

char* CLResponse::encMessage(string command, string user, string message){
    Message stMessage;

    memset(&stMessage, 0, sizeof(Message));
    stMessage.timestamp = this->getTimestamp();
    strncpy(stMessage.command, command.c_str(), sizeof(stMessage.command) - 1);
    strncpy(stMessage.user, user.c_str(), sizeof(stMessage.user) - 1);
    strncpy(stMessage.message, message.c_str(), sizeof(stMessage.message) - 1);
    
    // Serializar a struct em um array de bytes
    char* buffer = new char[sizeof(struct Message)];
    memcpy(buffer, &stMessage, sizeof(struct Message));

    return buffer;
}

char* CLResponse::encMessage(Message stMessage){
 
    // Serializar a struct em um array de bytes
    char* buffer = new char[sizeof(struct Message)];
    memcpy(buffer, &stMessage, sizeof(struct Message));

    return buffer;
}

Message CLResponse::decMessage(char* buffer){    
    struct Message stMessage;

    memset(&stMessage, 0, sizeof(stMessage));
    memcpy(&stMessage, buffer, sizeof(struct Message));

    return stMessage;
}
