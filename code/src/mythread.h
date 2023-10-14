#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>
#include <QString>
#include <QVector>

class TaskThread
{
private:
    size_t threadId;

    static long long unsigned int totalComputed;

    PcoMutex mutex;

    QVector<unsigned int> startPassword;

    QString passwordFound;

public:
    bool hasFound;

    static long long unsigned int getTotalComputed();

    TaskThread(size_t id);

    QString getPasswordFound();

    void taskHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbValidChars,
        long long unsigned int nbToCompute);
};

#endif // MYTHREAD_H
