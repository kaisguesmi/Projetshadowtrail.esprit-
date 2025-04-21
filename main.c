#include "puzzle.h"
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 1000
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
int main() {
    SDL_Surface* screen = NULL;
    
    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO) < 0 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur initialisation: %s\n", SDL_GetError());
        return 1;
    }
     // Initialisation de SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur d'initialisation de SDL_mixer : %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }
    // Création fenêtre
    screen = SDL_SetVideoMode(800, 650, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if (!screen) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    // Initialisation TTF
    if (TTF_Init() == -1) {
        printf("Erreur initialisation TTF: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Chemins des images (à adapter)
    const char* piece_paths[9] = {
        "0.png", "1.png", "2.png",
        "3.png", "4.png", "5.png",
        "6.png", "7.png", "8.png"
    };

    // Initialisation jeu
    PuzzleGame game;
    puzzle_init(&game, piece_paths);
    puzzle_load_font(&game, "arial.ttf", 24);

    // Boucle principale
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            puzzle_handle_event(&game, &event);
        }

        puzzle_update(&game);

        // Rendu
        
        puzzle_render(&game, screen);
        SDL_Flip(screen);
       

        if (game.state != PUZZLE_STATE_RUNNING) {
            SDL_Delay(2000);
            running = 0;
        }
         if(game.need_redraw) {
        puzzle_render(&game, screen);
        SDL_Flip(screen);
        game.need_redraw = false;
    }


        SDL_Delay(16);
    }

    // Nettoyage
    puzzle_cleanup(&game);
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}

