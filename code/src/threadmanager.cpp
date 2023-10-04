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

    for(size_t i = 0; i < nbThreads; ++i){
        //PcoThread *currentThread = new PcoThread(runTask, nbIterations);
                    //threadList.push_back(std::unique_ptr<PcoThread>(currentThread));

        PcoThread *currentThread = new PcoThread(taskHacking,i, charset, salt, hash, nbChars);
        threads.push_back(std::unique_ptr<PcoThread>(currentThread));
    }

    for(size_t i = 0; i < threads.size(); i++){
        threads.at(i)->join();
        std::cout << "thread ID " << i << std::endl;
    }



    threads.clear();

//je    /*
//je    unsigned int i;
//je
//je    long long unsigned int nbToCompute;
//je    long long unsigned int nbComputed;
//je
//je    /*
//je     * Nombre de caractères différents pouvant composer le mot de passe
//je     */
//je    unsigned int nbValidChars;
//je
//je    /*
//je     * Mot de passe à tester courant
//je     */
//je    QString currentPasswordString;
//je
//je    /*
//je     * Tableau contenant les index dans la chaine charset des caractères de
//je     * currentPasswordString
//je     */
//je    QVector<unsigned int> currentPasswordArray;
//je
//je    /*
//je     * Hash du mot de passe à tester courant
//je     */
//je    QString currentHash;
//je
//je    /*
//je     * Object QCryptographicHash servant à générer des md5
//je     */
//je    QCryptographicHash md5(QCryptographicHash::Md5);
//je
//je    /*
//je     * Calcul du nombre de hash à générer
//je     */
//je    nbToCompute        = intPow(charset.length(),nbChars);
//je    nbComputed         = 0;
//je
//je    /*
//je     * Nombre de caractères différents pouvant composer le mot de passe
//je     */
//je    nbValidChars       = charset.length();
//je
//je    /*
//je     * On initialise le premier mot de passe à tester courant en le remplissant
//je     * de nbChars fois du premier caractère de charset
//je     */
//je    currentPasswordString.fill(charset.at(0),nbChars);
//je    currentPasswordArray.fill(0,nbChars);
//je
//je    /*
//je     * Tant qu'on a pas tout essayé...
//je     */
//je    while (nbComputed < nbToCompute) {
//je        /* On vide les données déjà ajoutées au générateur */
//je        md5.reset();
//je        /* On préfixe le mot de passe avec le sel */
//je        md5.addData(salt.toLatin1());
//je        md5.addData(currentPasswordString.toLatin1());
//je        /* On calcul le hash */
//je        currentHash = md5.result().toHex();
//je
//je        /*
//je         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
//je         */
//je        if (currentHash == hash)
//je            return currentPasswordString;
//je
//je        /*
//je         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
//je         * de l'état de notre avancement (pour la barre de progression)
//je         */
//je        if ((nbComputed % 1000) == 0) {
//je            incrementPercentComputed((double)1000/nbToCompute);
//je        }
//je
//je        /*
//je         * On récupère le mot de pass à tester suivant.
//je         *
//je         * L'opération se résume à incrémenter currentPasswordArray comme si
//je         * chaque élément de ce vecteur représentait un digit d'un nombre en
//je         * base nbValidChars.
//je         *
//je         * Le digit de poids faible étant en position 0
//je         */
//je        i = 0;
//je
//je        while (i < (unsigned int)currentPasswordArray.size()) {
//je            currentPasswordArray[i]++;
//je
//je            if (currentPasswordArray[i] >= nbValidChars) {
//je                currentPasswordArray[i] = 0;
//je                i++;
//je            } else
//je                break;
//je        }
//je
//je        /*
//je         * On traduit les index présents dans currentPasswordArray en
//je         * caractères
//je         */
//je        for (i=0;i<nbChars;i++)
//je            currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));
//je
//je        nbComputed++;
//je    }
//je    */
//je    /*
//je     * Si on arrive ici, cela signifie que tous les mot de passe possibles ont
//je     * été testés, et qu'aucun n'est la préimage de ce hash.
//je     */
    return QString("abcd");
}
