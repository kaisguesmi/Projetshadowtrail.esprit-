#include "headers.h"
#include <SDL/SDL.h>

void InitialiserMinimap(minimap *m, int resize_percent) {
    m->mini_background = IMG_Load("background_mini.jpg");
    if (!m->mini_background) {
        printf("Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    m->icon = IMG_Load("icon.png");
    if (!m->icon) {
        printf("Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    m->position_minimap.x = 10;
    m->position_minimap.y = 10;
    m->position_icon.x = 0;
    m->position_icon.y = 0;
}
void MAJMinimap(SDL_Rect posJoueur, minimap *m, SDL_Rect camera, int resize_percent) {
    SDL_Rect pos_abs;
    pos_abs.x = posJoueur.x + camera.x;
    pos_abs.y = posJoueur.y + camera.y;

    // Calcul des coordonnées RELATIVES à la mini-map
    m->position_icon.x = (pos_abs.x * resize_percent) / 100;
    m->position_icon.y = (pos_abs.y * resize_percent) / 100;
}

void afficherMinimap(minimap m, SDL_Surface *screen) {
    // Afficher la mini-map à sa position
    SDL_BlitSurface(m.mini_background, NULL, screen, &m.position_minimap);
    
    // Calculer la position ABSOLUE de l'icône
    SDL_Rect icon_pos_abs;
    icon_pos_abs.x = m.position_minimap.x + m.position_icon.x;
    icon_pos_abs.y = m.position_minimap.y + m.position_icon.y;
    
    // Afficher l'icône à la position ajustée
    SDL_BlitSurface(m.icon, NULL, screen, &icon_pos_abs);
}

void LibererMinimap(minimap *m) {
    SDL_FreeSurface(m->mini_background);
    SDL_FreeSurface(m->icon);
}
