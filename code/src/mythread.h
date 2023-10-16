#ifndef MYTHREAD_H
/**
\file mythread.h
\author Eva Ray, Benoit Delay
\date 04.10.2023


Ce fichier contient la définition de la classe TaskThread, qui permet de
défnir la routine d'une tâche de hacking d'un hash md5.
*/
#define MYTHREAD_H

#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>
#include <QString>
#include <QVector>

class TaskThread
{
private:
    /**
     * @brief threadId id du thread qui gère la tâche
     */
    size_t threadId;

    /**
     * @brief Nombre de hash générés
     */
    long long unsigned nbHashComputed;

    /**
     * @brief passwordFound QString contient le mot de passe en clair 
     * correspondant au hash fourni
     */
    QString passwordFound;

    /**
     * @brief hasFoundPassword true si la tâche a cracké le hash
     */
    bool hasFoundPassword;

public:
    /**
     * @brief TaskThread constructeur simple
     * @param id du thread
     */
    TaskThread(size_t id);

    /**
     * @brief permet de récupérer le mot de passe trouvé depuis l'extérieur de 
     * la classe
     * @return le mot de passe trouvé
     */
    QString getPasswordFound();

    /**
     * @brief permet de récupérer le nombre de hash calculés dans la tâche
     * @return le nombre de hash calculés
     */
    long long unsigned getNbHashComputed();

    /**
     * @brief permet de savoir si le hash a été cracké
     * @return true si le hash a été cracké
     */
    bool isPasswordFound();

    /**
     * @brief taskHacking détermine la routine de la tâche de hacking
     * @param charset QString tous les caractères possibles composant le mot de
     * passe
     * @param salt QString sel qui permet de modifier dynamiquement le hash
     * @param hash QString hash à reverser
     * @param nbChars taille du mot de passe
     * @param nbValidChars nombre de caractères présents dans le charset
     * @param nbToCompute nombre de hash qu'un certain thread doit calculer
     */
    void taskHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbValidChars,
        long long unsigned int nbToCompute);
};

#endif // MYTHREAD_H
