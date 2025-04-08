#ifndef INTERFACE1_H
#define INTERFACE1_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#define SCREEN_W 1920
#define SCREEN_H 1080
#define MAX_NAME_LENGTH 50

// Structure pour gérer le texte (label et zone de saisie)
typedef struct {
    SDL_Surface *txt;       // Surface pour le rendu du texte
    SDL_Rect pos_txt;       // Position d’affichage
    SDL_Color color_txt;    // Couleur du texte
    TTF_Font *police;       // Police utilisée
    char name[MAX_NAME_LENGTH];  // Chaîne de saisie du pseudo
    SDL_Surface *textSurface; // Surface pour le texte saisi
} Texte;

// Structure pour l’arrière-plan
typedef struct {
    SDL_Surface *T[6];         // Tableaux d’images (ici, seule T[0] est utilisée)
    SDL_Rect pos_img_affiche;  // Position de l’image affichée
    Mix_Music *music;          // (Non utilisé ici)
    int niv;
} Background;

// Structure pour le bouton de confirmation
typedef struct {
    SDL_Surface *TB[2];        // TB[0] = état normal, TB[1] = état survolé
    SDL_Rect pos_img_ecran;    // Position du bouton
    int etat;                  // État du bouton (0 ou 1)
} Button;

// Prototypes des fonctions
void initInterface1(Background *bg, Button *btn, Mix_Chunk **sound, Texte *label, Texte *inputText);
void displayInterface1(SDL_Surface *screen, Background *bg, Button *btn, Texte *label, Texte *inputText);
int handleInterface1Events(Button *btn, int numButtons, Mix_Chunk *sound, Texte *inputText);
void freeInterface1(Background *bg, Button *btn, Mix_Chunk *sound, Texte *label, Texte *inputText);

// Fonction d’exécution de l’interface 1 qui retourne 1 pour passer à l’interface 2, ou 0 pour quitter
int runInterface1(SDL_Surface *screen);

#endif // INTERFACE1_H

