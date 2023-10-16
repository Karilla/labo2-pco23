# Rapport Laboratoire 2 PCO: Cracking MD5

Auteurs: Benoit Delay, Eva Ray

## Description des fonctionnalités du logiciel

Le programme que nous avons développé permet de cracker un hash md5 par bruteforce, afin de récupérer un mot de passe. Une application de base nous a 
été fournie mais elle souffrait de problèmes de performances car elle n'était pas multi-threadée. Pour améliorer son efficacité, nous avons donc intégré une fonctionnalité de multithreading.

Afin d'utiliser le programme, l'utilisateur doit fournir un hash md5, la longueur du mot de passe cherché et le nombre de threads qu'il souhaite utiliser. Le sel est également une option que l'utilisateur peut fournir, si nécessaire.  

Lors de l'exécution du programme, une barre de progression est affichée et se remplit graduellement. Cette bare représente le pourcentage de mots de passe potentiels qui ont déjà été testés. 

## Choix d'implémentation

<!--Comment avez-vous abordé le problème, quels choix avez-vous fait, quelle 
décomposition avez-vous choisie, quelles variables ont dû être protégées, ...-->

L'objectif principal de l'implémentation du multi-threading était d'amméliorer les performances de l'application, tout en gardant une cohérence dans les variables partagées.

Notre stratégie pour répartir la charge de travail entre les threads est la suivante. Nous considérons le nombre total des mots de passe possible et nous divisons par le nombre de threads fourni par l'utilisateur. Le résultat représente donc le nombre de mots de passe que chaque thread va devoir tester.

```cpp
nbToCompute = intPow(charset.length(), nbChars);
/*
* Calcul du nombre de hash à générer par thread
* On ajoute 1 pour le cas où nbToCompute n'est pas divisible par nbThreads
*/
nbToComputePerThread = (nbToCompute / nbThreads) + 
                        (nbToCompute % nbThreads ? 1 : 0);
```

Pour implémenter cette stratégie, nous avons décidé de créer une classe *TaskThread* qui contient tous les attibuts et méthodes nécessaires à la définition de la routine qu'un thread doit mettre en place pour procéder au cracking de mots de passe. Nous avons opté pour une classe plutôt qu'une simple fonction globale, afin de profiter des capacités d'encapsulation du C++. Cela nous a aussi permis de travailler avec des attributs statiques pour les variables partagées entre les threads plutôt que des variables globales.

C'est la méthode *TaskThread::taskHacking* qui contient la routine de cracking. La première étape est de trouver le mot de passe à partir duquel le thread doit commencer à tester les hash. Pour cela, la méthode reçoit le paramètre *nbToCompute* qui représente le nombre de mots de passe que le thread doit tester. A partir de ce paramètre et de l'id du thread, on déduit la position du mot de passe à partir duquel commencer.

```cpp
long long unsigned startPosition = threadId * nbToCompute;
```
A partir de ce mot de passe et jusqu'à avoir atteint *nbToCompute* itérations, le thread va donc calculer le hash du mot de passe et le comparer avec celui fournit par l'utilisateur. S'ils sont identiques, l'attribut *hasFound* de l'objet *TaskThread* va être actualisé à *true* et le contenu de l'attibut *passwordFound* va être actualisé avec la mot de passe trouvé.

```cpp
if (currentHash == hash){
    passwordFound = currentPasswordString;
    hasFound = true;
}
```

C'est la classe *ThreadManager* qui s'occupe de gérer les threads. Nous avons ajouté un attribut qui permet de stocker les threads lancés.

```cpp
std::vector<std::unique_ptr<PcoThread>> threads;
```

C'est à l'intérieur de la méthode *ThreadManager::startHacking* qu'on va gérer les threads. On commence par lancer les threads et, grâce au getter *isPasswordFound* des objets *TaskThread*, on peut donner l'ordre à tous les threads de s'arrêter lorsqu'un mot de passse a été trouvé. On récupère alors le mot de passe stocké dans l'attribut *passwordFound*, qui sera la retour de la fonction *ThreadManager::startHacking*.

```cpp
 if (taskThreads.at(i)->isPasswordFound())
    {
        passwordFound = taskThreads.at(i)->getPasswordFound();

        // Demande aux threads de s'arreter
        for (size_t j = 0; j < threads.size(); j++)
        {
            threads.at(j)->requestStop();
        }
    }
```
La dernière fonctionnalité qui doit être gérée est la barre de progression. Nous avons opté pour un attribut statique *totalComputed* dans la classe *TaskThread* qui contient le nombre total de mots de passe déjà testés, tous threads confondus. Chaque thread met à jour cet attribut dans la méthode *taskHacking* lorsqu'un nouveau mot de passe a été testé et c'est à l'intérieur de la méthode *ThreadManager::startHacking* que la barre de progression est mise à jour en utilisant cette information.

```cpp
if ((TaskThread::getTotalComputed() % 1000) == 0)
{
    incrementPercentComputed((double)1000 / nbToCompute);
}
```
Comme plusieurs threads vont vouloit accéder à cet attribut statique en même temps, nous avons dû le protéger. Pour ce faire, nous utilisions un *PcoMutex* qu'on vérouille juste avant l'incrémentation de l'attribut et qu'on dévérouille juste après.

```cpp
mutex.lock();
totalComputed++;
mutex.unlock();
```

## Tests effectués

Les tests sont effectués sur la machine virtuelle du REDS, à qui nous avons alloué 4 coeurs. Chaque cas de test a été lancé trois fois et les résultats affichés dans les tableaux ci-dessous sont la moyenne de ces tentatives. Le benchmark brut est disponible dans l'annexe à la fin de ce rapport.

### Test 1: Mot de passe de longueur 4 sans sel

Commençons avec un mot de passe de longueur 4, sans ajouter de sel. Choisissons "abcd" qui est donc plutôt en début de dictionnaire.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | 370.7 |
| 2 | 564 |
| 4 | 1001 |
| 8 | 1636.33 |

Faisons un autre test avec le mot de passe "ABCD" qui est plutôt en milieu de dictionnaire.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | 3442 |
| 2 | 4447 |
| 4 | 3295|
| 8 | 2952.67 |

Faisons un autre test avec le mot de passe "!!!!" qui est plutôt en fin de dictionnaire.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | 7620.33 |
| 2 | 4895 |
| 4 | 3991 |
| 8 | 2499 |

Nous constatons que la vitesse d'exécution du programme dépend fortement du placement du mot de passe à trouver dans l'espace des mots de passe possibles. En effet, avec un seul thread, le meilleur cas est un mot de passe en début de dictionnaire et le pire cas un mot de passe en fin de dictionnaire.

Dans le meilleur cas, il est donc logique que, avec notre stratégie de répartition de l'espace entre les threads, ajouter plus de threads ralentisse le programme. En effet, nous perdons du temps en lançant plusieurs threads et en faisant plusieurs fois la mise en place de la tâche de hacking.

Dans le pire cas, il est logique qu'ajouter plus de threads amméliore les performances. En effet, bien que le mot de passe reste en fin d'espace même après séparation entre les threads, chaque thread a bien moins de mots de passe à parcourir. Il atteint donc le dernier plus vite. 

Dans le cas moyen, la situation est moins tranchée. On pourrait penser que plus il y a de threads, plus les performances sont bonnes mais ce n'est pas forcément le cas. Bien que cette tendance se dégage en effet des résultat des tests, cela dépend encore une fois de comment l'espace des mots de passe est réparti entre les threads. Si le mot de passe cherché se retrouve en fin d'un sous-ensemble, alors il sera trouvé moins vite que s'il se retrouve au début. C'est d'ailleurs ce qu'il se passe dans le cas avec deux threads.

Tous les comportements énumérés ci-dessus sont des comportements attendus. Les résultats des tests sont cohérents.

De plus, pendant d'autres tests sur notre VM, nous avons parfois constaté que, dès 8 threads, les performances commencent à diminuer. Cela est dû à la configuration de notre VM, à qui nous avons pu allouer 4 coeurs. Cela implique qu'à partir de 8 threads, il commence à y avoir beaucoup de préemptions, ce qui ralentit le programme. C'est un comportement attendu.

### Test 2: Mot de passe de longueur 4 avec sel

Testons le mot de passe "abdc" avec le sel "xy".

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | 367.67 |
| 2 | 556.33 |
| 4 | 892.33 |
| 8 | 1775 |

Nous constatons que les résultats sont extrêmement similaires à ceux observés pour ce même mot de passe lors du test 1. C'est tout à fait logique puisque, une fois que nous avons précisé le sel utilisé à notre programme, le problème revient en fait à cracker un mot de passe de longueur 4 à nouveau. C'est un comportement attendu.

### Test 3: Mot de passe de longueur 5 sans sel

Testons avec un mot de passe de longueur 5, sans ajouter de sel. Choisissons "abcde" qui se trouve plutôt en début de dictionnaire.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | 33428.33 |
| 2 | 48639 |
| 4 | 90511.33 |

Nous constatons que le temps d'exécution du programme est bien plus élevé qu'avec un mot de passe de longueur 4. C'est logique car avec une longueur de 4, nous testons au plus $(66)^4 = 18'974'736$ mots de passe, alors qu'avec une longueur de 5, nous testons au plus $(66)^5 = 1'252'332'576 $ mots de passe. En général, la technique de crackage de mots de passe bruteforce est très lente pour de longs mots de passe. Notre programme n'est donc pas adapté pour cracker un long mot de passe.

Ceci dit, nous constatons que plus il y a de threads, plus l'exécution du programme est lente, ce qui est le comportement attendu pour un mot de passe se trouvant au début du dictionnaire.

## Conclusion

Nous avons pu constater que le temps que met notre programme à cracker un mot de passe dépend fortement de la position de celui-ci dans le dictionnaire. Ceci étant dit, notre programme se comporte de façon attendue en fonction de cette position. Nous avons aussi observé qu'ajouter un sel n'impactait pas les performances du programme et que notre programme était lent pour cracker de longs mots de passe. Pour finir, nous pouvons dire que nous avons réussi à amméliorer de manière significative les performances de l'application de base.