#include <iostream>
#include <string>
#include <vector>
#include "User.h"

using namespace std;


struct STMessage{
    string message;
    bool flagRead;
    User* sender;
    User* recipient;
};



class Message{
private:
    vector<STMessage> messages;

public:
    Message(string message, User* sender, User* recipient);
    ~Message();
};