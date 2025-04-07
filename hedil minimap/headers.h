#ifndef HEADERS_H
#define HEADERS_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Structure minimap
typedef struct {
    SDL_Surface *mini_background;  // Image miniature
    SDL_Rect position_minimap;     // Position de la mini-map
    SDL_Surface *icon;             // Icône du joueur
    SDL_Rect position_icon;        // Position de l'icône
} minimap;

// Prototypes de fonctions
void InitialiserMinimap(minimap *m, int resize_percent);
void MAJMinimap(SDL_Rect posJoueur, minimap *m, SDL_Rect camera, int resize_percent);
void afficherMinimap(minimap m, SDL_Surface *screen);
void LibererMinimap(minimap *m);

#endif
