#include "GerThreads.h"

GerThreads::GerThreads(/* args */){
}

GerThreads::~GerThreads(){
}



/**
 * ADD THREAD
 * 
 * A função cria uma thread vinculada ao usuário ou
 * ao sistema e insere ao final do vetor de threads. 
 * 
 * @param thread* th
 * @param User* user
 * @param string desc
 * 
 * @return bool
 * 
*/
bool GerThreads::addThread(thread* th, User* user, string desc){
    bool flagThread = false;
    int numThreads = this->threads.size();
    
    STDescThread newThread(th, user, desc);
    this->threads.push_back(newThread);

    // Validação da inserção da thread
    if(this->threads.size() > numThreads){
        flagThread = true;
    }

    return flagThread;
}


void GerThreads::joinAll(){
    for(STDescThread stThread : this->threads){
        stThread.th->join();
    }
}