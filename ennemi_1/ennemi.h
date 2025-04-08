/*
 * ennemi.h - Définition des structures et prototypes pour le module ennemi
 */
#ifndef ENNEMI_H
#define ENNEMI_H

#include <SDL/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Constantes pour les états de santé de l'ennemi
#define ENNEMI_VIVANT    0
#define ENNEMI_BLESSE    1
#define ENNEMI_NEUTRALISE 2

// Constantes pour les types d'animation
#define ANIM_DEPLACEMENT 0
#define ANIM_ATTAQUE     1

// Structure pour les animations avec images individuelles
typedef struct {
    SDL_Surface **frames;  // Tableau de pointeurs vers chaque frame
    int frame_actuelle;
    int nb_frames;
    int delai_frame;
    int compteur_delai;
    int direction;         // Direction associée à cette animation
} Animation;

// Structure pour les entités ES (Entity System)
typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
    int actif;
    int vitesse;
} ES;

// Structure principale pour l'ennemi
typedef struct {
    SDL_Rect position;
    int vitesse_x;
    int vitesse_y;
    int direction;        // 0: gauche, 1: droite, 2: haut, 3: bas
    int etat_sante;       // 0: vivant, 1: blessé, 2: neutralisé
    Animation **animations[2]; // [type][direction] - animations par type et direction
    SDL_Surface *images_sante[3]; // Images correspondant aux états de santé
    int niveau;           // 1 ou 2 selon le niveau du jeu
    int est_en_attaque;   // Flag pour l'animation d'attaque
    ES *entites_es;       // Tableau d'entités ES
    int nb_entites_es;    // Nombre d'entités ES
    int temps_changement_direction; // Temps avant changement de direction
    int compteur_temps;   // Compteur pour le changement de direction
} Ennemi;

// Prototypes des fonctions
Animation* initialiser_animation(const char *prefixe_image, int direction, int nb_frames, int delai);
Ennemi* initialiser_ennemi(int niveau, int nb_entites_es);
void liberer_ennemi(Ennemi *ennemi);
void afficher_ennemi(Ennemi *ennemi, SDL_Surface *ecran);
void deplacer_ennemi(Ennemi *ennemi, SDL_Rect limites);
void animer_ennemi(Ennemi *ennemi);
int detecter_collision_joueur_ennemi(SDL_Rect pos_joueur, Ennemi *ennemi);
int detecter_collision_joueur_es(SDL_Rect pos_joueur, Ennemi *ennemi);
void gerer_sante_ennemi(Ennemi *ennemi, int dommage);

#endif // ENNEMI_H
