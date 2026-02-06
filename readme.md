Rapport de Projet : Ultimate Tic-Tac-Toe
Algorithmique Avancee - L3 Informatique
Date : Janvier 2026

Mathis HAMRI


1- Introduction

Ce projet implemente le jeu de l'Ultimate Tic-Tac-Toe en langage C. L'objectif principal etait de developper une Intelligence Artificielle capable de jouer de maniere competitive en utilisant les algorithmes Minimax et Alpha-Beta, tout en respectant les regles complexes de deplacement entre les sous-grilles.

2- Structures de Donnees et Fonctions Principales

2.1- Structure GameState
Pour representer l'etat du jeu a un instant T, nous utilisons une structure contenant :
- int grille[9][9] : Le plateau global contenant les pions (0=vide, 1=J1, 2=J2).
- int sous_grille[3][3] : Indique l'etat de chaque sous-grille (0=en cours, 1=gagnee par J1, 2=gagnee par J2).
- int next_r, next_c : Les coordonnees de la contrainte imposee pour le prochain coup (-1 si coup libre).

2.2- Gestion de la Contrainte (jouer_coup et est_coup_valide)
La regle specifique de l'Ultimate Tic-Tac-Toe impose que le coup joue dans une case locale (r, c) envoie l'adversaire dans la sous-grille correspondante.
- Implementation : Si la sous-grille cible est deja gagnee ou pleine, la contrainte est levee (next_r = -1), offrant un coup libre a l'adversaire. Cela empeche de jouer dans des zones deja resolues, comme la consigne le veut.
- Mode "Sans Contrainte" : Un mode optionnel (activable via #define) permet de desactiver cette regle.

2.3- Fonction d'Evaluation (evaluer_position)
L'IA ne pouvant explorer l'arbre jusqu'a la fin de la partie (profondeur limitee), une fonction estime la qualite d'un plateau. Elle se base sur trois criteres ponderes :
1. Victoire Globale (+/- 10 000) : Priorite absolue.
2. Controle des Sous-Grilles (+/- 500) : Gagner un carre local est un objectif intermediaire majeur.
3. Fonction auxiliaire (+/- 10) : Au sein d'une sous-grille non terminee, la fonction evaluer_petite_grille favorise l'alignement de 2 pions sans blocage adverse, encourageant l'IA a construire des attaques.

3- Algorithmes d'Intelligence Artificielle

3.1- Minimax
L'algorithme de base explore recursivement l'arbre des coups possibles.
- Principe : Maximiser son gain tout en supposant que l'adversaire minimise ce meme gain.
- Limitation : La complexite exponentielle rend le calcul lent des que la profondeur depasse 4 ou 5, surtout lorsque le coup est "libre" (81 branches possibles).

3.2- Alpha-Beta (Optimisation)
Version optimisee de Minimax implementee pour ameliorer les performances.
- Elagage : L'algorithme cesse d'explorer une branche des qu'il trouve une preuve qu'elle est pire qu'une solution deja trouvee ailleurs (Coupure Alpha ou Beta).
- Rapidité : Permet d'atteindre la meme decision optimale que Minimax mais beaucoup plus rapidement, ou d'explorer plus profondement dans le meme temps imparti.

4- Tests et Comparaisons de Performance

4.1- Protocole de Test
Un mode "Demo" a ete implemente pour faire s'affronter :
- Joueur 1 : IA Minimax (Profondeur faible ou egale).
- Joueur 2 : IA Alpha-Beta (Profondeur elevee ou egale).
Le temps de reflexion est mesure via clock().

4.2- Resultats Observes
- Situation contrainte (Petite grille) : Les deux algorithmes sont rapides (< 0.1s) car l'arbre est petit (9 choix max).
- Situation libre (Plateau entier) :
    - Minimax (Prof 4) : Peut prendre plusieurs secondes (> 5s).
    - Alpha-Beta (Prof 4) : Reste quasi-instantane (< 0.5s).
    - Alpha-Beta (Prof 6) : Reste jouable (< 3s), la ou Minimax gelerait le programme.

4.3- Conclusion des Tests
L'elagage Alpha-Beta est indispensable pour ce jeu. Il permet a l'IA de jouer de maniere fluide tout en maintenant une profondeur de recherche suffisante (5 ou 6 coups) pour anticiper les pieges strategiques, comme le fait de sacrifier une case locale pour envoyer l'adversaire dans une sous-grille deja perdue.


Points Bonus Implementes :
- Affichage colore pour la lisibilite (Rouge/Cyan).
- Affichage "Grand Symbole" lorsqu'une sous-grille est gagnee.
- Gestion correcte des matchs nuls (Grille pleine).
- Menu de selection pour le mode Demo vs Humain.
