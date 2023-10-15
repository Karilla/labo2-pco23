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

### Test 1: Mot de passe de longueur 4, sans sel

Commençons avec un mot de passe de longueur 4, sans ajouter de sel. Choisissons "abcd" qui est donc plutôt en début de dictionnaire.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | Texte |
| 2 | Texte |
| 4 | Texte |
| 8 | Texte |

Faisons un autre test avec le mot de passe "momo" qui est plutôt en milieu de dictionnaire.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | Texte |
| 2 | Texte |
| 4 | Texte |
| 8 | Texte |

Nous constatons que, jusqu'à 4 threads, plus y a de threads, plus le programme est rapide. C'est le comportement que nous espérons! 

Cependant, dès 8 threads, les performances commencent à diminuer. Cela est dû à la configuration de notre VM, à qui nous avons pu allouer X coeurs. Cela implique qu'à partir de 8 threads, il commence à y avoir beaucoup de préemption, ce qui ralentit le programme. C'est un comportement attendu.

On constate aussi que la vitesse d'exécution du programme dépend du placement du mot de passe à trouver dans l'espace des mots de passe possibles. S'il est au début, il est logique que même avec un seul thread, l'exécution soit rapide. S'il est plus loin dans le dictionnaire, le temps d'exécution avec un thread va être plus long car on doit passer par tous les mots de passe intermédiaires. Il y a donc des cas de mots de passe plus favorables que d'autres quant à la rapidité d'exécution de notre programme.

### Test 2: Mot de passe de longueur 4, avec sel

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | Texte |
| 2 | Texte |
| 4 | Texte |
| 8 | Texte |

### Test 3: Mot de passe de longueur 4, sans sel, sans mutex

Testons le même cas que le test 1 avec "abcd" mais enlevons cette fois-ci le mutex qui protège l'attribut statique *totalComputed* qui permet de gérer correctement la barre de progression.

 nombre threads | temps avec mutex [ms] |temps sans mutex [ms] |
| :---: | :---: | :---: |
| 1 | Texte | Texte |
| 2 | Texte | Texte |
| 4 | Texte | Texte |
| 8 | Texte | Texte |

On constate que sans le mutex, notre programme est bien plus rapide (presque deux fois plus)! On constate aussi que plus il y a de threads, plus la différence de temps entre la version sans mutex et avec mutex grandit. Cela semble logique car plus il y a de threads, plus la "file d'attente" pour accéder à la variable protégée est grande, puisque plus de threads veulent y accéder en même temps.

Cette observation est intéressante car on peut constater à quel point protéger des variables est coûteux en terme de performances. Il est donc important de comprendre quelle situation implique de devoir absolument protéger une variable. Dans notre cas, si on ne protège pas notre variable, la barre de progression n'a plus aucun sens. On ne peut donc pas échapper au mutex. Bien que cela ralentisse notre programme, la cohérence des données prime.  

### Test 4: Mot de passe de longueur 5, sans sel

Testons avec un mot de passe de longueur 5, sans ajouter de sel.

 nombre threads | temps [ms] 
| :---: | :---: |
| 1 | Texte |
| 2 | Texte |
| 4 | Texte |

Nous constatons que le temps d'exécution du programme est bien plus élevé qu'avec un mot de passe de longueur 4. C'est logique car avec une longueur de 4, nous testons au plus $(66)^4 = 18'974'736$ mots de passe, alors qu'avec une longueur de 5, nous testons au plus $(66)^5 = 1'252'332'576 $ mots de passe. En général, la technique de crackage de mots de passe bruteforce est très lente pour de longs mots de passe.

Ceci dit, nous constatons que plus il y a de threads, plus l'exécution du programme est rapide, ce qui est le comportement attendu.

<!--Description de chaque test, et information sur le fait qu'il ait passé ou non-->

## Conclusion

Nous avons pu constater 