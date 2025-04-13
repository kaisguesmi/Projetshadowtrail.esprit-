#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "bg.h"

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDL_Surface *screen=SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    SDL_WM_SetCaption("SplitScreen", NULL);

    Background bg1, bg2;
    int level1 = 1, level2 = 1;

    bg_init(&bg1, level1, 1); // écran 1
    bg_init(&bg2, level2, 2); // écran 2

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;
        const Uint8 *keys = SDL_GetKeyState(NULL);

        bg_update(&bg1, keys, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT);
        bg_update(&bg2, keys, SDLK_z, SDLK_s, SDLK_q, SDLK_d);

        // Niveau 2 pour écran 1 (X)
        if (level1 == 1 && bg_get_camera_x(&bg1) > 4800 - VIEW_W && keys[SDLK_x]) {
            level1 = 2;
            bg_free(&bg1);
            bg_init(&bg1, level1, 1);
        }

        // Niveau 2 pour écran 2 (C)
        if (level2 == 1 && bg_get_camera_x(&bg2) > 4800 - VIEW_W && keys[SDLK_c]) {
            level2 = 2;
            bg_free(&bg2);
            bg_init(&bg2, level2, 2);
        }

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 100, 100, 100));
        SDL_Rect sep = {VIEW_W - 2, 0, 4, SCREEN_HEIGHT};
        SDL_FillRect(screen, &sep, SDL_MapRGB(screen->format, 0, 0, 0));

        bg_render(&bg1, screen, 0);
        bg_render(&bg2, screen, VIEW_W);

        SDL_Flip(screen);
        SDL_Delay(16);
    }

    bg_free(&bg1);
    bg_free(&bg2);
    TTF_Quit(); IMG_Quit(); SDL_Quit();
    return 0;
}
