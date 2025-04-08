#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "interface1.h"
#include "interface2.h"
#include "enigme.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }
    SDL_EnableUNICODE(1);
    
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de SDL_ttf : %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur d'initialisation de SDL_mixer : %s\n", Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        printf("Erreur : Impossible de créer la fenêtre.\n");
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Exécution de l'interface 1
    int result = runInterface1(screen);
    if (result == 1) {
        // Exécution de l'interface 2
        result = runInterface2(screen);
        // Si l'utilisateur appuie sur 'e', runInterface2 retourne 2 : on lance alors l'interface 3
        if (result == 2) {
            runEnigme(screen);
        }
    }
    
    SDL_FreeSurface(screen);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

