#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pcosynchro/pcothread.h>
#include <QString>

void taskHacking(int threadId,QString charset, QString salt, QString hash, unsigned int nbChar);

#endif // MYTHREAD_H
