#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "bg.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!screen) {
        fprintf(stderr, "SDL_SetVideoMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_WM_SetCaption("Platform Game", NULL);

    Uint32 global_start_time = SDL_GetTicks();
    Background bg;
    int level = 1;
    init_background(&bg, level, global_start_time);

    int running = 1;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = 0;
            }
            else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_x && level == 1 && bg.texts[2].is_showing) {
                    free_background(&bg);
                    level = 2;
                    init_background(&bg, level, global_start_time);
                }
                if (ev.key.keysym.sym == SDLK_ESCAPE) {
                    bg.show_final_time = 1;
                }
            }
        }

        Uint8 *keystate = SDL_GetKeyState(NULL);
        update_scrolling(&bg, keystate);
        update_platforms(&bg);

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        render_all(&bg, screen);
        SDL_Flip(screen);
        SDL_Delay(16);
    }

    free_background(&bg);
    SDL_Quit();
    return 0;
}

