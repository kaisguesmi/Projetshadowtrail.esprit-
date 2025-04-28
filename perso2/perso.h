// perso.h
#ifndef PERSONNE_H
#define PERSONNE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h> // Include TTF header

#define SCREEN_W 1200
#define SCREEN_H 720
#define GROUND_Y 500

#define NB_ACTIONS 6      // 0: walk_r, 1: walk_l, 2: attack, 3: jump, 4: run_r, 5: run_l (Reserve slot 5 even if unused)
#define NB_FRAMES 4       // Max nombre d'images par action (assumed uniform)
#define LIFE_STATES 4     // 0: pleine vie ... 3: état final (mort)
#define INVINCIBILITY_DURATION 1000 // Durée d'invincibilité en millisecondes (1 seconde)


// --- Added Animation Intervals (in milliseconds) ---
#define WALK_FRAME_INTERVAL 150 // Faster walking animation
#define RUN_FRAME_INTERVAL 100  // Even faster running animation
#define JUMP_FRAME_INTERVAL 180 // Slower jump animation
#define ATTACK_FRAME_INTERVAL 100 // Fast attack animation frames
#define IDLE_FRAME_INDEX 0      // Frame to show when idle

// --- Threshold for switching to run animation ---
#define RUN_SPEED_THRESHOLD 150 // Speed above which run animation is used

typedef struct
{
    SDL_Surface *images[NB_ACTIONS][NB_FRAMES];
    SDL_Surface *life_images[LIFE_STATES];
    SDL_Surface *death_sprite; // Image quand le perso est mort
    SDL_Rect position;
    int direction;            // 0 = droite, 1 = gauche
    int nbr_frame;
    Uint32 last_frame_time;   // timestamp dernière mise à jour de l'animation actuelle

    // --- Specific Animation Intervals ---
    Uint32 walk_interval;
    Uint32 run_interval;
    Uint32 jump_interval;
    Uint32 attack_interval; // Interval between frames *during* attack

    Uint32 last_attack_time;  // Timestamp when the last attack was *initiated* (for cooldown)
    Uint32 attack_cooldown;   // Minimum time between attacks

    double vitesse;           // Horizontal speed (pixels per second)
    double vitesse_saut;      // Vertical velocity (pixels per second)
    int up;                   // 0 = au sol, 1 = saut/chute en cours
    int life_state;           // 0..LIFE_STATES-1 index vie
    int is_attacking;         // Flag: 1 if attack animation is playing, 0 otherwise
    int score;                // Player's score
    Uint32 last_hit_time;       // Timestamp du dernier coup reçu
    int is_dead;             // 0 = vivant, 1 = mort

} personne;

// Initialisation
void initialiserperso(personne* p);
void initialiserperso2(personne* p1);

// Affichage et déplacement
void afficherperso(const personne p, SDL_Surface *screen);
void affichervie(const personne p, SDL_Surface *screen, int x, int y);
void afficherScore(const personne p, SDL_Surface *screen, TTF_Font *font, int x, int y); // Score display
void moveperso(personne *p, Uint32 dt);

// Animations
void updateAnimation(personne* p, Uint32 now);

// Actions & Physics
void saut(personne* p, int ground_y, Uint32 dt);
void startAttack(personne* p, Uint32 now);

// Vie
void perdre_vie(personne* p);

// Cleanup
void freePerso(personne *p); // Function to free allocated surfaces

#endif // PERSONNE_H
