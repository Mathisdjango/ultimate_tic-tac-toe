#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <limits.h> 

#define L 9
#define C 9
#define P 5
#define MODE_CONTRAINTE 1

typedef struct {
    int grille[L][C];        
    int sous_grille[3][3];   
    int next_r;     
    int next_c;   
} GameState;



void init_game(GameState *state) {
    for(int i = 0; i < L; i++)
        for(int j = 0; j < C; j++)
            state->grille[i][j] = 0; 

    for(int i = 0; i < 3; i++)    
        for(int j = 0; j < 3; j++)
            state->sous_grille[i][j] = 0; 

    state->next_r = -1; 
    state->next_c = -1;
}

void afficher_jeu(GameState *state){
    printf("\n");
    printf("      0 1 2   3 4 5   6 7 8\n");
    printf("    +-------+-------+-------+\n");

    for(int i=0; i<L; i++){
        printf("%d   | ", i); // Numéro de ligne
        for(int j=0; j<C; j++){
            
            int s_g_r = i / 3;
            int s_g_c = j / 3;
            int winner = state->sous_grille[s_g_r][s_g_c];

            // SI LA SOUS-GRILLE EST GAGNÉE
            if (winner != 0) {
                int lr = i % 3; // Ligne locale (0, 1 ou 2)
                int lc = j % 3; // Colonne locale (0, 1 ou 2)

                if (winner == 1) {
                    // Dessin d'un Grand X en ROUGE
                    // On affiche sur les diagonales locales
                    if (lr == lc || lr + lc == 2) { //lr == lc :(0,0), (1,1), (2,2) lr + lc == 2 :(0,2), (1,1), (2,0).
                        printf("\033[31mX\033[0m ");
                    } else {
                        printf("  "); // Espace vide pour faire ressortir la croix
                    }
                } 
                else if (winner == 2) {
                    // Dessin d'un Grand O en CYAN
                    // On affiche aux coordonnées (0,1), (1,0), (1,2), (2,1)
                    if ((lr + lc) % 2 == 1) {
                        printf("\033[36mO\033[0m ");
                    } else {
                        printf("  ");
                    }
                }
            }
            // SINON ON AFFICHE LE CONTENU NORMAL
            else {
                if (state->grille[i][j] == 1) {
                    printf("\033[31mX\033[0m "); 
                }
                else if (state->grille[i][j] == 2) {
                    printf("\033[36mO\033[0m "); 
                }
                else {
                    printf(". ");
                }
            }

            // Séparateur vertical tous les 3 blocs
            if ((j + 1) % 3 == 0) printf("| ");
        }
        printf("\n");

        // Séparateur horizontal tous les 3 blocs
        if ((i + 1) % 3 == 0) {
            printf("    +-------+-------+-------+\n");
        }
    }
    
    if (state->next_r != -1) {
        printf("\nProchain coup impose dans le carre : (%d, %d)\n", state->next_r, state->next_c);
    } else {
        printf("\nCoup libre (vous pouvez jouer dans n'importe quel carre non plein)\n");
    }
}

// Vérifie si un joueur a gagné dans une grille 3x3 donnée
int check_win(int data[3][3], int player) {
    // Lignes et Colonnes
    for (int i = 0; i < 3; i++) {
        if (data[i][0] == player && data[i][1] == player && data[i][2] == player) return 1;
        if (data[0][i] == player && data[1][i] == player && data[2][i] == player) return 1;
    }
    // Diagonales
    if (data[0][0] == player && data[1][1] == player && data[2][2] == player) return 1;
    if (data[0][2] == player && data[1][1] == player && data[2][0] == player) return 1;
    return 0;
}

// Vérifie la victoire dans une sous-grille spécifique du plateau
int check_subgrid_win(GameState *state, int s_g_r, int s_g_c, int player) {
    int temp[3][3];
    // On extrait la sous-grille 3x3
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            temp[i][j] = state->grille[s_g_r*3 + i][s_g_c*3 + j];
        }
    }
    return check_win(temp, player);
}

// Vérifie si une sous-grille est pleine
int is_subgrid_full(GameState *state, int s_g_r, int s_g_c) {
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            if (state->grille[s_g_r*3 + i][s_g_c*3 + j] == 0) return 0;
        }
    }
    return 1;
}

int est_coup_valide(GameState *state, int r, int c) {
    // limite de la grille
    if (r < 0 || r >= 9 || c < 0 || c >= 9) return 0;
    
    // case vide
    if (state->grille[r][c] != 0) return 0;

    // vérifier si la sous-grille est déjà gagnée 
    int s_g_r = r / 3;
    int s_g_c = c / 3;
    if (state->sous_grille[s_g_r][s_g_c] != 0) return 0; // Interdit de jouer ici !

    // respect de la contrainte (si elle existe)
    if (state->next_r != -1) {
        // si on ne joue pas dans la bonne grande grille -> Invalide
        if (s_g_r != state->next_r || s_g_c != state->next_c) return 0;
    }

    return 1;
}

void jouer_coup(GameState *state, int r, int c, int player, int debug) {
    // placer le symbole
    state->grille[r][c] = player;

    int s_g_r = r / 3;
    int s_g_c = c / 3;

    // vérifier si ce coup gagne la sous-grille (si elle n'est pas déjà gagnée)
    if (state->sous_grille[s_g_r][s_g_c] == 0) { 
        if (check_subgrid_win(state, s_g_r, s_g_c, player)) { 
            state->sous_grille[s_g_r][s_g_c] = player;
            if( debug == 1){
                printf(">>> Le joueur %d gagne la sous-grille (%d, %d) !\n", player, s_g_r, s_g_c);
            }
        }
    }

    if (MODE_CONTRAINTE == 1) {
        int next_s_g_r = r % 3;
        int next_s_g_c = c % 3;

        if (is_subgrid_full(state, next_s_g_r, next_s_g_c) || state->sous_grille[next_s_g_r][next_s_g_c] != 0) { 
            state->next_r = -1;
            state->next_c = -1;
        } else {
            state->next_r = next_s_g_r;
            state->next_c = next_s_g_c;
        }
    } else {
        // Mode sans contrainte : toujours libre
        state->next_r = -1;
        state->next_c = -1;
    }
}

// vérifie la victoire globale
int check_global_win(GameState *state, int player) {
    return check_win(state->sous_grille, player);
} 

// Fonction auxiliaire pour évaluer la domination dans une petite grille 3x3 
int evaluer_petite_grille(int grille[3][3], int player) {
    int score = 0;
    int adv;
    if (player == 1) {
        adv = 2;
    } else {
        adv = 1;
    }

    // Lignes, Colonnes, Diagonales
    int lignes[8][3] = {
        {grille[0][0], grille[0][1], grille[0][2]}, // L1
        {grille[1][0], grille[1][1], grille[1][2]}, // L2
        {grille[2][0], grille[2][1], grille[2][2]}, // L3
        {grille[0][0], grille[1][0], grille[2][0]}, // C1
        {grille[0][1], grille[1][1], grille[2][1]}, // C2
        {grille[0][2], grille[1][2], grille[2][2]}, // C3
        {grille[0][0], grille[1][1], grille[2][2]}, // D1
        {grille[0][2], grille[1][1], grille[2][0]}  // D2
    };

    for (int i = 0; i < 8; i++) {
        int nb_p = 0; // nombre de pions du joueur
        int nb_adv = 0; // nombre de pions de l'adversaire
        for (int j = 0; j < 3; j++) {
            if (lignes[i][j] == player) nb_p++;
            if (lignes[i][j] == adv) nb_adv++;
        }

        // Si j'ai 2 pions et que la 3ème case est vide -> Gros bonus (coup gagnant imminent)
        if (nb_p == 2 && nb_adv == 0) score += 10;
        // Si l'adversaire a 2 pions et que la 3ème est vide -> Malus (danger)
        if (nb_adv == 2 && nb_p == 0) score -= 10;
        // Si j'ai 1 pion et le reste vide -> Petit bonus
        if (nb_p == 1 && nb_adv == 0) score += 1;
    }
    return score;
}

int evaluer_position(GameState *state) {
    // 1. Victoire globale (Priorité absolue)
    if (check_global_win(state, 1)) return 10000;
    if (check_global_win(state, 2)) return -10000;

    int score = 0;

    // 2. Évaluation des sous-grilles gagnées (Méta-grille)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (state->sous_grille[i][j] == 1) score += 500;      // Une sous-grille vaut beaucoup
            else if (state->sous_grille[i][j] == 2) score -= 500;
            else if (state->sous_grille[i][j] == 0) {
                // 3. Si la sous-grille n'est pas finie, on regarde le détail
                // On extrait la petite grille temporaire
                int temp[3][3];
                for(int r=0; r<3; r++) {
                    for(int c=0; c<3; c++) {
                        temp[r][c] = state->grille[i*3 + r][j*3 + c];
                    }
                }
                score += evaluer_petite_grille(temp, 1);
            }
        }
    }
    return score;
}

int max(int a, int b) {
    if (a > b) return a;
    else return b;
}

int min(int a, int b) {
    if (a < b) return a;
    else return b;
}

// algo minimax
int minimax(GameState state, int profondeur, int joueur) {
    //quel est le score actuel 
    int score = evaluer_position(&state);
    //cas d'arrêt
    if (abs(score) == 10000 || profondeur == 0) {
        return score;
    }

    // génération des coups possibles
    int r_min = 0, r_max = 9;
    int c_min = 0, c_max = 9;
    // si le coup est imposé
    if (state.next_r != -1) {
        r_min = state.next_r * 3; r_max = r_min + 3;
        c_min = state.next_c * 3; c_max = c_min + 3;
    }

    // logique MAX / MIN 
    if (joueur == 1) { // MAX
        int meilleur_score = -1000000; // comme ça n'importe quel score sera plus grand
        int coup_possible = 0;

        for (int r = r_min; r < r_max; r++) {
            for (int c = c_min; c < c_max; c++) {
                if (est_coup_valide(&state, r, c)) {
                    coup_possible = 1;
                    GameState next_state = state; 
                    jouer_coup(&next_state, r, c, 1, 0);

                    int val = minimax(next_state, profondeur - 1, 2);
                    meilleur_score = max(meilleur_score, val);
                }
            }
        }
        if (!coup_possible) return score;
        return meilleur_score;
    } 
    else { // MIN (joueur == 2)
        int meilleur_score = 1000000; 
        int coup_possible = 0;

        for (int r = r_min; r < r_max; r++) {
            for (int c = c_min; c < c_max; c++) {
                if (est_coup_valide(&state, r, c)) {
                    coup_possible = 1;
                    GameState next_state = state;
                    jouer_coup(&next_state, r, c, 2, 0);

                    int val = minimax(next_state, profondeur - 1, 1);
                    meilleur_score = min(meilleur_score, val);
                }
            }
        }
        if (!coup_possible) return score;
        return meilleur_score;
    }
}


// Algorithme Alpha-Bêta
int alpha_beta(GameState state, int profondeur, int joueur, int alpha, int beta) {
    int score = evaluer_position(&state);
    if (abs(score) == 10000 || profondeur == 0) {
        return score;
    }

    int r_min = 0, r_max = 9;
    int c_min = 0, c_max = 9;

    if (state.next_r != -1) {
        r_min = state.next_r * 3; r_max = r_min + 3;
        c_min = state.next_c * 3; c_max = c_min + 3;
    }

    if (joueur == 1) { // MAX
        int meilleur_score = -1000000;
        int coup_possible = 0;

        for (int r = r_min; r < r_max; r++) {
            for (int c = c_min; c < c_max; c++) {
                if (est_coup_valide(&state, r, c)) {
                    coup_possible = 1;
                    GameState next_state = state; 
                    jouer_coup(&next_state, r, c, 1, 0);

                    int val = alpha_beta(next_state, profondeur - 1, 2, alpha, beta);
                    meilleur_score = max(meilleur_score, val);
                    
                    alpha = max(alpha, meilleur_score);
                    if (beta <= alpha) return meilleur_score; // Coupure
                }
            }
        }
        if (!coup_possible) return score;
        return meilleur_score;
    } 
    else { // MIN
        int meilleur_score = 1000000;
        int coup_possible = 0;

        for (int r = r_min; r < r_max; r++) {
            for (int c = c_min; c < c_max; c++) {
                if (est_coup_valide(&state, r, c)) {
                    coup_possible = 1;
                    GameState next_state = state;
                    jouer_coup(&next_state, r, c, 2, 0);

                    int val = alpha_beta(next_state, profondeur - 1, 1, alpha, beta);
                    meilleur_score = min(meilleur_score, val);

                    beta = min(beta, meilleur_score);
                    if (beta <= alpha) return meilleur_score; // Coupure
                }
            }
        }
        if (!coup_possible) return score;
        return meilleur_score;
    }
}

// use_ab : 1 pour Alpha-Bêta, 0 pour Minimax classique
void jouer_ia(GameState *state, int profondeur, int joueur_actuel, int use_ab) {
    int meilleur_score;
    int best_r = -1;
    int best_c = -1;

    int alpha = -1000000;
    int beta = 1000000;

    int adversaire;
    if (joueur_actuel == 1) {
        meilleur_score = -1000000; // MAX
        adversaire = 2;
    } else {
        meilleur_score = 1000000;  // MIN
        adversaire = 1;
    }
    
    int r_min = 0, r_max = 9, c_min = 0, c_max = 9;
    if (state->next_r != -1) {
        r_min = state->next_r * 3; r_max = r_min + 3;
        c_min = state->next_c * 3; c_max = c_min + 3;
    }

    for (int r = r_min; r < r_max; r++) {
        for (int c = c_min; c < c_max; c++) {
            if (est_coup_valide(state, r, c)) {
                
                GameState next_state = *state; 
                jouer_coup(&next_state, r, c, joueur_actuel, 0); 
                         
                int score;
                if (use_ab == 1) {
                    score = alpha_beta(next_state, profondeur - 1, adversaire, alpha, beta);
                } else {
                    score = minimax(next_state, profondeur - 1, adversaire);
                }

                if (joueur_actuel == 1) { // MAX
                    if (score > meilleur_score) { 
                        meilleur_score = score;
                        best_r = r;
                        best_c = c;
                    }
                    if (use_ab == 1) {
                        if (meilleur_score > alpha) {
                            alpha = meilleur_score;
                        }
                    }
                } else { // MIN
                    if (score < meilleur_score) {
                        meilleur_score = score;
                        best_r = r;
                        best_c = c;
                    }
                    if (use_ab == 1) {
                        if (meilleur_score < beta) {
                            beta = meilleur_score;
                        }
                    }
                }
            }
        }
    }
    
    if (best_r != -1) {
        if (use_ab == 1) {
            printf("[Alpha-Beta] ");
        } else {
            printf("[Minimax] ");
        }
        
        printf("J%d joue en (%d, %d) -> Eval: %d\n", joueur_actuel, best_r, best_c, meilleur_score);
        jouer_coup(state, best_r, best_c, joueur_actuel, 1);
    } else {
        // ICI : Gestion du cas où l'IA ne peut plus jouer
        printf("L'IA ne trouve pas de coup valide. Partie probablement terminee (Match nul ?).\n");
    }
}

int main() { 
    GameState jeu;
    init_game(&jeu);
    
    int joueur = 1;
    int running = 1; 
    int mode_jeu = 0;
    int r, c;

    printf("=== ULTIMATE TIC TAC TOE ===\n");
    printf("1. Humain (X) vs IA (O)\n");
    printf("2. Demo : IA Minimax (X) vs IA Alpha-Beta (O)\n");
    printf("Votre choix : ");
    scanf("%d", &mode_jeu);

    while(running) {
        afficher_jeu(&jeu);
        
        // Mesure du temps de réflexion
        clock_t start = clock();

        if (joueur == 1) {
            // --- TOUR JOUEUR 1 ---
            if (mode_jeu == 1) {
                // Mode Humain
                printf("\nVotre tour (Ligne Colonne) : ");
                int valid = 0;
                
                // Vérification préventive : est-ce qu'il existe un coup valide ?
                int coup_possible = 0;
                for(int i=0; i<L; i++) {
                    for(int j=0; j<C; j++) {
                        if (est_coup_valide(&jeu, i, j)) {
                            coup_possible = 1;
                            break;
                        }
                    }
                    if(coup_possible) break;
                }
                
                if (!coup_possible) {
                    printf("\nAucun coup possible pour vous !\n");
                    running = 0; // On force la sortie pour aller checker le match nul
                } else {
                    while (!valid) {
                        if (scanf("%d %d", &r, &c) != 2) {
                            while(getchar() != '\n'); 
                            printf("Entree invalide. Reessayez : ");
                            continue;
                        }
                        if (est_coup_valide(&jeu, r, c)) {
                            valid = 1;
                            jouer_coup(&jeu, r, c, 1, 1);
                        } else {
                            printf("Coup invalide ! Verifiez la contrainte.\nReessayez : ");
                        }
                    }
                }
            } else {
                // Mode Démo : IA Minimax (use_ab = 0)
                printf("\n--- Tour Joueur 1 (IA Minimax) ---\n");
                jouer_ia(&jeu, 3, 1, 1); 
            }
        } else {
            // --- TOUR JOUEUR 2 ---
            // Mode IA Alpha-Bêta (use_ab = 1)
            printf("\n--- Tour Joueur 2 (IA Alpha-Beta) ---\n");
            jouer_ia(&jeu, 6, 2, 1);
        }

        // Affichage du temps si c'est une IA qui a joué
        if (mode_jeu == 2 || joueur == 2) {
            clock_t end = clock();
            double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
            printf("Temps de reflexion : %.4f secondes\n", time_spent);
        }

        if (check_global_win(&jeu, joueur)) {
            afficher_jeu(&jeu);
            printf("\n!!! LE JOUEUR %d A GAGNE !!!\n", joueur);
            running = 0;
            
        } else {
            // vérification de match nul (grille pleine)
            int plein = 1;
            for(int i=0; i<L; i++) {
                for(int j=0; j<C; j++) {
                    if(jeu.grille[i][j] == 0) {
                        plein = 0;
                        break;
                    } 
                }
                if(!plein) break;
            }

            if (plein) {
                afficher_jeu(&jeu);
                printf("\n!!! MATCH NUL (GRILLE PLEINE) !!!\n");
                running = 0;
            } else {
                // Changement de joueur classique
                if (joueur == 1) {
                    joueur = 2;
                } else {
                    joueur = 1;
                }
            }
        }
    }
    return 0;
}