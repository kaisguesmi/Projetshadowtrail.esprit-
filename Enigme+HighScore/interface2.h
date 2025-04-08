#ifndef INTERFACE2_H
#define INTERFACE2_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#define SCREEN_W 1920
#define SCREEN_H 1080
#define NUM_BUTTONS 2

// Structure pour les boutons de l’interface 2
typedef struct {
    SDL_Surface *TB[2];   // TB[0] = état normal, TB[1] = état survolé
    int etat;
    SDL_Rect pos_img_ecran;
} Button2;

// Structure pour l’arrière-plan
typedef struct {
    SDL_Surface *T[6];
    SDL_Rect pos_img_affiche;
    Mix_Music *music;
    int niv;
} Background2;

// Structure pour le texte
typedef struct {
    TTF_Font *police;
    SDL_Color color_txt;
    SDL_Rect pos_txt;
    SDL_Surface *txt;
} Texte2;

// Structure pour les étoiles
typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
} Etoile;

// Prototypes des fonctions
void initInterface2(Background2 *bg, Button2 *buttons, Mix_Chunk **sound, Texte2 *label, Texte2 *texte2, Texte2 *texte3, Etoile *etoile1, Etoile *etoile2, Etoile *etoile3);
void displayInterface2(SDL_Surface *screen, Background2 *bg, Button2 *buttons, Texte2 *label, Texte2 *texte2, Texte2 *texte3, Etoile *etoile1, Etoile *etoile2, Etoile *etoile3);
int handleInterface2Events(Button2 *buttons, int numButtons, Mix_Chunk *sound);
void freeInterface2(Background2 *bg, Button2 *buttons, Mix_Chunk *sound, Texte2 *label, Texte2 *texte2, Texte2 *texte3, Etoile *etoile1, Etoile *etoile2, Etoile *etoile3);

// Fonction d’exécution de l’interface 2
int runInterface2(SDL_Surface *screen);

#endif // INTERFACE2_H

