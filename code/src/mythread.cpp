/**
\file mythread.cpp
\author Eva Ray, Benoit Delay
\date 15.10.2023


Ce fichier contient l'implémentation de la classe TaskThread, qui permet de
défnir la routine d'une tâche de hacking d'un hash md5.
*/
#include "mythread.h"
#include "threadmanager.h"
#include <QCryptographicHash>
#include <QVector>
#include <QObject>
#include <iostream>
#include <pcosynchro/pcologger.h>

/*
 * Initilisation de l'attribut statique
 */
long long unsigned int TaskThread::totalPasswordsComputed = 0;

TaskThread::TaskThread(size_t id) : threadId(id)
{
    hasFoundPassword = false;
    passwordFound = "";
}

void TaskThread::taskHacking(
    QString charset,
    QString salt,
    QString hash,
    unsigned int nbChars,
    unsigned int nbValidChars,
    long long unsigned int nbToCompute)
{
    /*
     * Variable d'itération
     */
    unsigned int i = 0;

    /*
     * Position dans le dictionnaire du mot de passe à partir duquel on
     * commence à tester
     */
    long long unsigned startPosition = threadId * nbToCompute;

    /*
     * Mot de passe à tester courant
     */
    QString currentPasswordString;

    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
    QVector<unsigned int> currentPasswordArray;

    /*
     * Hash du mot de passe à tester courant
     */
    QString currentHash;

    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);

    /*
     * Nombre de hash générés
     */
    long long unsigned nbComputed = 0;

    /*
     * On initialise le premier mot de passe à tester courant en le remplissant
     * de nbChars fois du premier caractère de charset
     */
    currentPasswordString.fill(charset.at(0), nbChars);
    currentPasswordArray.fill(0, nbChars);

    /*
     * Calcul du mot de passe de départ
     */
    long long unsigned remainingChar = startPosition;
    while (i < (unsigned int)currentPasswordArray.size())
    {
        currentPasswordArray[i] += remainingChar;
        if (currentPasswordArray[i] >= nbValidChars)
        {
            remainingChar = currentPasswordArray[i] / nbValidChars;
            currentPasswordArray[i] %= nbValidChars;
            i++;
        }
        else
        {
            break;
        }
    }
    for (unsigned int i = 0; i < nbChars; i++)
        currentPasswordString[i] = charset.at(currentPasswordArray.at(i));

    /*
     * Tant qu'on a pas tout essayé...
     */
    while (nbComputed < nbToCompute)
    {
        if (PcoThread::thisThread()->stopRequested())
        {
            return;
        }
        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         */
        if (currentHash == hash)
        {
            passwordFound = currentPasswordString;
            hasFoundPassword = true;
        }
        /*
         * On récupère le mot de pass à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArray comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */
        i = 0;

        while (i < (unsigned int)currentPasswordArray.size())
        {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars)
            {
                currentPasswordArray[i] = 0;
                i++;
            }
            else
                break;
        }

        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i = 0; i < nbChars; i++)
            currentPasswordString[i] = charset.at(currentPasswordArray.at(i));

        nbComputed++;

        /*
         * Incrémentation d'attribut statique demandant un vérouillage
         */
        mutex.lock();
        totalPasswordsComputed++;
        mutex.unlock();
    }
    /*
     * On a calculé le nombre de hash assigné et on n'a rien trouvé, on s'arrête
     */
    return;
}

QString TaskThread::getPasswordFound()
{
    return passwordFound;
}

long long unsigned int TaskThread::getTotalComputed()
{
    return totalPasswordsComputed;
}

bool TaskThread::isPasswordFound(){
    return hasFoundPassword;
}
