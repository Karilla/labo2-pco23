#include "mythread.h"
#include <iostream>
#include <pcosynchro/pcologger.h>

void taskHacking(int threadId, QString charset, QString salt, QString &hash, unsigned int nbChar){
    logger() << "Bonjour je suis le thread numero " << threadId << std::endl;
    if(PcoThread::thisThread()->stopRequested()){
        return;
    }
    return;
}
