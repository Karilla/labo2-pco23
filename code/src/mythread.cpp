#include "mythread.h"
#include "threadmanager.h"
#include <QCryptographicHash>
#include <QVector>
#include <QObject>
#include <iostream>
#include <pcosynchro/pcologger.h>


long long unsigned int TaskThread::totalComputed = 0;

TaskThread::TaskThread(QVector<unsigned int> startPosition):startPassword(startPosition){
    hasFound = false;
    passwordFound = "";
}

void TaskThread::taskHacking(
                 QString charset,
                 QString salt,
                 QString hash,
                 unsigned int nbChars,
                 unsigned int nbValidChars,
                 long long unsigned int nbToCompute
                 )
{
    /*logger() << "Bonjour je suis le thread numero " << threadId << std::endl;
    if(PcoThread::thisThread()->stopRequested()){
        return;
    }*/

    unsigned int i;
    long long unsigned int nbComputed;

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
    nbComputed         = 0;

    /*
     * On initialise le premier mot de passe à tester courant en le remplissant
     * de nbChars fois du premier caractère de charset
     */
    currentPasswordString.fill(charset.at(0),nbChars);
    currentPasswordArray = startPassword;

    for (unsigned int i=0;i<nbChars;i++)
        currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));


    /*
     * Tant qu'on a pas tout essayé...
     */
    while (nbComputed < nbToCompute) {
        //logger() << "Ca compute un max" << std::endl;
        if(PcoThread::thisThread()->stopRequested()){
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
        if (currentHash == hash){
            passwordFound = currentPasswordString;
            hasFound = true;
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

        while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars) {
                currentPasswordArray[i] = 0;
                i++;
            } else
                break;
        }

        /*
             * On traduit les index présents dans currentPasswordArray en
             * caractères
             */
        for (i=0;i<nbChars;i++)
            currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));

        nbComputed++;
        mutex.lock();
        totalComputed++;
        mutex.unlock();
    }


    return;
}

QString TaskThread::getPasswordFound(){
    return passwordFound;
}

long long unsigned int TaskThread::getTotalComputed(){
    return totalComputed;
}
