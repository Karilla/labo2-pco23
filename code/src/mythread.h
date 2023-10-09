#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pcosynchro/pcothread.h>
#include <QString>

class TaskThread{
private:
    int counter;
public:
    TaskThread();

    TaskThread(int i);

    void task();
};

void taskHacking(int threadId,
                 QString charset,
                 QString salt,
                 QString hash,
                 unsigned int nbChars,
                 unsigned int nbValidChars,
                 long long unsigned int nbToCompute,
                 long long unsigned int startNb,
                 QString& passwordFound);

#endif // MYTHREAD_H
