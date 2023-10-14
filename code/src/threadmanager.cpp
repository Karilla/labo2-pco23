#include <QCryptographicHash>
#include <QVector>
#include <iostream>
#include <pcosynchro/pcologger.h>

#include "threadmanager.h"
#include "mythread.h"

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow (
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

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{}


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
        unsigned int nbThreads
        )
{
    logger().setVerbosity(1);

    unsigned int i = 0;
    long long unsigned int totalPasswordComputed;
    long long unsigned int nbToCompute;
    long long unsigned int nbToComputePerThread;
//    long long unsigned int nbComputed;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;
    /*
     * Mot de passe à tester courant
     */
    //QString currentPasswordString;

    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
     QVector<unsigned int> currentPasswordArray;

    /*
     * Hash du mot de passe à tester courant
     */
//    QString currentHash;

    /*
     * Object QCryptographicHash servant à générer des md5
     */
//    QCryptographicHash md5(QCryptographicHash::Md5);

    /*
     * Calcul du nombre de hash à générer
     */
     nbToCompute = intPow(charset.length(),nbChars);
    nbToComputePerThread        = (nbToCompute / nbThreads) + 1;
    long long unsigned nbComputed         = 0;
    /*
         * Nombre de caractères différents pouvant composer le mot de passe
         */
    nbValidChars       = charset.length();

    /*
     * String dans laquelle stocker le mot de passe trouvé
     */
    QString passwordFound;




    /*
         * On initialise le premier mot de passe à tester courant en le remplissant
         * de nbChars fois du premier caractère de charset
         */
//    currentPasswordString.fill(charset.at(0),nbChars);
      currentPasswordArray.fill(0,nbChars);

    /*
         * Tant qu'on a pas tout essayé...
         */
//    while (nbComputed < nbToCompute) {
//        /* On vide les données déjà ajoutées au générateur */
//        md5.reset();
//        /* On préfixe le mot de passe avec le sel */
//        md5.addData(salt.toLatin1());
//        md5.addData(currentPasswordString.toLatin1());
//        /* On calcul le hash */
//        currentHash = md5.result().toHex();

//        /*
//             * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
//             */
//        if (currentHash == hash)
//            return currentPasswordString;

//        /*
//             * Tous les 1000 hash calculés, on notifie qui veut bien entendre
//             * de l'état de notre avancement (pour la barre de progression)
//             */
//

//        /*
//             * On récupère le mot de pass à tester suivant.
//             *
//             * L'opération se résume à incrémenter currentPasswordArray comme si
//             * chaque élément de ce vecteur représentait un digit d'un nombre en
//             * base nbValidChars.
//             *
//             * Le digit de poids faible étant en position 0
//             */
        // i = 0;
        QVector<QVector<unsigned int>> startPositions;
        while(nbComputed < nbToCompute){
            //logger() <<nbComputed <<"/" <<nbToCompute <<std::endl;
        while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars) {
                currentPasswordArray[i] = 0;
                i++;
            } else
                break;         

            }
        if(!(nbComputed % nbToComputePerThread))
        {
            startPositions.push_back(currentPasswordArray);
        }

        nbComputed++;
        }
            /*
             * On traduit les index présents dans currentPasswordArray en
             * caractères
             */
//        for (i=0;i<nbChars;i++)
//            currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));


        QVector<TaskThread*> taskThreads;
        /*
         * On lance les threads
         */
        for(size_t i = 0; i < nbThreads; ++i){
            TaskThread* taskThread = new TaskThread(startPositions.at(i));
            taskThreads.push_back(taskThread);
            PcoThread *currentThread = new PcoThread(&TaskThread::taskHacking, taskThread,charset, salt, hash, nbChars, nbValidChars, nbToCompute);
            threads.push_back(std::unique_ptr<PcoThread>(currentThread));
        }
        bool hasFound = false;
        while(!hasFound){
            if ((TaskThread::getTotalComputed() % 1000) == 0) {
                incrementPercentComputed((double)1000/nbToCompute);
            }
            for(size_t i = 0; i < threads.size(); i++){
                if(taskThreads.at(i)->hasFound){
                    hasFound = true;
                    passwordFound = taskThreads.at(i)->getPasswordFound();
                    for(size_t j = 0; j < threads.size(); j++){
                        threads.at(j)->requestStop();
                    }
                }
            }
        }


        for(size_t i = 0; i < threads.size(); i++){
            threads.at(i)->join();
            std::cout << "thread ID " << i << std::endl;
        }

        threads.clear();

    if(passwordFound.length()){
        return passwordFound;
    }

    /*
         * Si on arrive ici, cela signifie que tous les mot de passe possibles ont
         * été testés, et qu'aucun n'est la préimage de ce hash.
         */
    return QString("");
}
