#include "headers.h"
#include <SDL/SDL.h>

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0) { // Initialisation de SDL_image
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Initialisation de la mini-map
    minimap m;
    InitialiserMinimap(&m, 20);  // 20% de la taille originale

    // Exemple de position joueur et caméra
    SDL_Rect posJoueur = {100, 100, 50, 50};
    SDL_Rect camera = {0, 0, 800, 600};

    int continuer = 1;
    SDL_Event event;

    while (continuer) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) continuer = 0;
        }

        // Mise à jour de la mini-map
        MAJMinimap(posJoueur, &m, camera, 20);

        // Effacer l'écran
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

        // Afficher la mini-map
        afficherMinimap(m, screen);

        // Mettre à jour l'écran
        SDL_Flip(screen);
    }

    // Libération des ressources
    LibererMinimap(&m);
    SDL_Quit();
    return 0;
}
