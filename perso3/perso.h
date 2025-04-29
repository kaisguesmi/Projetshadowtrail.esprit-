// perso.h
#ifndef PERSONNE_H
#define PERSONNE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h> // Include TTF header

#define SCREEN_W 1200
#define SCREEN_H 720
#define GROUND_Y 500 // Représente le NIVEAU DU SOL (où les pieds touchent)

#define NB_ACTIONS 6      // 0: walk_r, 1: walk_l, 2: attack, 3: jump, 4: run_r, 5: run_l
#define NB_FRAMES 4       // Max nombre d'images par action (assumed uniform)
#define LIFE_STATES 4     // 0: pleine vie ... 3: état final (mort)
#define INVINCIBILITY_DURATION 1000 // Durée d'invincibilité en millisecondes (1 seconde)


// --- Animation Intervals (in milliseconds) ---
#define WALK_FRAME_INTERVAL 150
#define RUN_FRAME_INTERVAL 100
#define JUMP_FRAME_INTERVAL 180 // Visuel seulement, la physique est continue
#define ATTACK_FRAME_INTERVAL 100
#define IDLE_FRAME_INDEX 0      // Frame to show when idle

// --- Movement/Physics Constants ---
#define RUN_SPEED_THRESHOLD 150
#define MAX_JUMPS 2             // Saut au sol + 1 saut aérien = 2
#define INITIAL_JUMP_VELOCITY -550.0 // Vitesse verticale initiale (pixels/sec, négatif = vers le haut)
#define GRAVITY (980.0 * 2.0)     // Gravité (pixels/sec^2), augmentée pour une sensation plus rapide

typedef struct
{
    SDL_Surface *images[NB_ACTIONS][NB_FRAMES];
    SDL_Surface *life_images[LIFE_STATES];
    SDL_Surface *death_sprite;
    SDL_Rect position;        // IMPORTANT: stocke le coin SUPÉRIEUR GAUCHE du sprite
    int direction;            // 0 = droite, 1 = gauche
    int nbr_frame;
    Uint32 last_frame_time;   // Pour l'animation

    // --- Animation Intervals ---
    Uint32 walk_interval;
    Uint32 run_interval;
    Uint32 jump_interval;
    Uint32 attack_interval;

    Uint32 last_attack_time;  // Pour cooldown attaque
    Uint32 attack_cooldown;

    double vitesse;           // Vitesse horizontale (pixels/sec)
    double vitesse_saut;      // Vitesse verticale (pixels/sec)
    int up;                   // 1 si en l'air (saut/chute), 0 au sol
    int jumps_remaining;      // Compteur pour double saut
    int life_state;
    int is_attacking;
    int score;
    Uint32 last_hit_time;       // Pour invincibilité
    int is_dead;

} personne;

// Initialisation
void initialiserperso(personne* p);
void initialiserperso2(personne* p1);

// Affichage et déplacement
void afficherperso(const personne p, SDL_Surface *screen); // Utilise p.position.y
void affichervie(const personne p, SDL_Surface *screen, int x, int y);
void afficherScore(const personne p, SDL_Surface *screen, TTF_Font *font, int x, int y);
void moveperso(personne *p, Uint32 dt);

// Animations
void updateAnimation(personne* p, Uint32 now);

// Actions & Physics
void saut(personne* p, int ground_y, Uint32 dt); // Gère gravité et atterrissage
void startAttack(personne* p, Uint32 now);

// Vie
void perdre_vie(personne* p); // C'est elle qui met is_dead à 1

// Cleanup
void freePerso(personne *p);

#endif // PERSONNE_H
