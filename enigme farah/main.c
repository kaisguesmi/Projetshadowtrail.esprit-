#include "puzzle.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!screen) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize puzzle game
    PuzzleGame game;
    puzzle_init(&game, "puzzle.png");
    puzzle_load_background(&game, "background.png");
    puzzle_load_font(&game, "arial.ttf", 24);

    // Main game loop
    SDL_Event event;
    int running = 1;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            puzzle_handle_event(&game, &event);
        }

        // Update game state
        puzzle_update(&game);

        // Render
        puzzle_render(&game, screen);
        SDL_Flip(screen);

        // Check if game is over
        if (game.state != PUZZLE_STATE_RUNNING) {
            SDL_Delay(2000); // Show win/lose message for 2 seconds
            running = 0;
        }

        // Cap frame rate
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    puzzle_cleanup(&game);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
