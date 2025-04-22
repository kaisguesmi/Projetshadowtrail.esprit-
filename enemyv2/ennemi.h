/*
ennemi.h - Définition des structures et prototypes pour le module ennemi
*/
#ifndef ENNEMI_H
#define ENNEMI_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h> // Include SDL_image
#include <stdlib.h>
#include <time.h>
#include <math.h> // Needed for sqrt, atan2 etc.

// Constantes pour les états de santé de l'ennemi (will be used for heart display)
#define ENNEMI_HEALTH_MAX 3
#define ENNEMI_HEALTH_FULL 3
#define ENNEMI_HEALTH_TWO 2
#define ENNEMI_HEALTH_ONE 1
#define ENNEMI_HEALTH_ZERO 0

// Constantes pour les types d'animation
#define ANIM_MOVE 0
#define ANIM_SHOOT 1

// Directions
#define DIR_LEFT 0
#define DIR_RIGHT 1

// --- NEW: AI Constants ---
#define ENNEMY_ATTACK_RANGE 300 // Horizontal pixel range to notice player
#define ENNEMY_BULLET_SPEED 4   // Pixels per frame for bullet

// Structure pour les animations avec images individuelles
typedef struct {
    SDL_Surface **frames; // Tableau de pointeurs vers chaque frame
    int frame_actuelle;
    int nb_frames;
    int delai_frame;
    int compteur_delai;
    int direction; // Direction associée à this animation (DIR_LEFT or DIR_RIGHT) - Usually DIR_RIGHT for source
} Animation;

// --- MODIFIED: Added Projectile struct definition ---
typedef struct {
    SDL_Rect pos;
    // --- Using floats for more accurate angled movement ---
    float real_x;
    float real_y;
    float vx; // Velocity x
    float vy; // Velocity y
    int active; // Is this projectile currently on screen?
    SDL_Surface* image; // Pointer to the image surface (loaded once in main)
} Projectile;

// Structure principale pour l'ennemi
typedef struct {
    SDL_Rect position;
    int vitesse_x; // Base speed when just patrolling
    int direction; // DIR_LEFT or DIR_RIGHT (current facing/moving direction)
    int health; // Current health (for heart display)
    Animation **animations; // Array of pointers [ANIM_TYPE * 2 + DIRECTION]
    int est_en_attaque; // Flag for attack state (player in range)
    int temps_changement_direction; // Timer duration for patrolling direction change
    int compteur_temps; // Current counter for direction change
    int shoot_timer; // Timer to control shooting frequency within attack state
    int shoot_delay; // Delay between shots when in attack state
} Ennemi;


// --- Function Prototypes ---
Animation* initialiser_animation(const char *prefixe_image, int nb_frames, int delai);
Ennemi* initialiser_ennemi(int niveau);
void liberer_ennemi(Ennemi *ennemi);
void afficher_ennemi(Ennemi *ennemi, SDL_Surface *ecran);

// --- MODIFIED: Needs player position for AI ---
void deplacer_ennemi(Ennemi *ennemi, SDL_Rect limites, SDL_Rect player_pos);

void animer_ennemi(Ennemi *ennemi);
int detecter_collision_joueur_ennemi(SDL_Rect pos_joueur, Ennemi *ennemi);
void gerer_sante_ennemi(Ennemi *ennemi, int dommage);

// --- MODIFIED: Needs player position for targeting ---
void ennemi_shoot(Ennemi *ennemi, Projectile *bullet, SDL_Rect player_pos);

#endif // ENNEMI_H
