/**
\file threadmanager.cpp
\author Yann Thoma, Eva Ray, Benoit Delay
\date 24.02.2017


Ce fichier contient l'implementation de la classe ThreadManager, qui permet de
reverser un hash md5 par brute force.
*/

#include <QCryptographicHash>
#include <QVector>
#include <iostream>
#include <pcosynchro/pcologger.h>

#include "threadmanager.h"
#include "mythread.h"

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow(
    long long unsigned int number,
    long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

ThreadManager::ThreadManager(QObject *parent) : QObject(parent)
{
}

void ThreadManager::incrementPercentComputed(double percentComputed)
{
    emit sig_incrementPercentComputed(percentComputed);
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
    QString charset,
    QString salt,
    QString hash,
    unsigned int nbChars,
    unsigned int nbThreads)
{
    long long unsigned int nbToCompute;
    long long unsigned int nbToComputePerThread;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;

    /*
     * Calcul du nombre de hash à générer
     */
    nbToCompute = intPow(charset.length(), nbChars);
    nbToComputePerThread = (nbToCompute / nbThreads) + 1;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    nbValidChars = charset.length();

    /*
     * String dans laquelle stocker le mot de passe trouvé
     */
    QString passwordFound;

    /*
     *  Vecteur des tâches de hacking
     */
    QVector<TaskThread *> taskThreads;

    /*
     * On lance les threads
     */
    for (size_t i = 0; i < nbThreads; ++i)
    {
        TaskThread *taskThread = new TaskThread(i);
        taskThreads.push_back(taskThread);
        PcoThread *currentThread = new PcoThread(&TaskThread::taskHacking, taskThread, charset, salt, hash, nbChars, nbValidChars, nbToComputePerThread);
        threads.push_back(std::unique_ptr<PcoThread>(currentThread));
    }

    /*
     * Tant que le mot de passe n'est pas trouvé
     */
    bool isPasswordFound = false;
    //long long unsigned onePercent = nbToCompute * 0.01;
    long long unsigned nbTotalHashComputed = 0;
    while (!isPasswordFound)
    {
        if ((nbTotalHashComputed % 1000) == 0)
        {
            incrementPercentComputed((double)1000 / nbToCompute);
        }
        nbTotalHashComputed = 0;
        for (const auto &task : taskThreads)
        {
            // Dès que le mot de passe est trouvé
            if (task->isPasswordFound())
            {
                isPasswordFound = true;
                passwordFound = task->getPasswordFound();

                // Demande aux threads de s'arreter
                for (const auto &thread : threads)
                {
                    thread->requestStop();
                }
            }
            nbTotalHashComputed += task->getNbHashComputed();
        }
    }

    for (size_t i = 0; i < threads.size(); i++)
    {
        threads.at(i)->join();
    }

    threads.clear();

    if (passwordFound.length())
    {
        return passwordFound;
    }

    /*
     * Si on arrive ici, cela signifie que tous les mot de passe possibles ont
     * été testés, et qu'aucun n'est la préimage de ce hash.
     */
    return QString("");
}
