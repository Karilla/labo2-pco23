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




## Tests effectués


<!--Description de chaque test, et information sur le fait qu'il ait passé ou non-->

## Conclusions