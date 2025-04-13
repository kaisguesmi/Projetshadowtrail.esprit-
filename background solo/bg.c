#include "bg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void init_timer(GameTimer *t, Uint32 start_ticks) {
    t->start_ticks = start_ticks;
    t->time_surface = NULL;
    t->total_time = 0;

    if (!TTF_WasInit() && TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    t->font = TTF_OpenFont("arial.ttf", TIMER_FONT_SIZE);
    if (!t->font) {
        fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

void update_timer(GameTimer *t) {
    Uint32 elapsed = SDL_GetTicks() - t->start_ticks;
    Uint32 seconds = elapsed / 1000;
    char buf[32];
    snprintf(buf, sizeof(buf), "Time: %02d:%02d", seconds / 60, seconds % 60);

    SDL_Color white = {255, 255, 255};
    if (t->time_surface) SDL_FreeSurface(t->time_surface);
    t->time_surface = TTF_RenderText_Solid(t->font, buf, white);

    t->total_time = elapsed;
}

static void draw_final_time(Background *bg, SDL_Surface *screen) {
    char buf[64];
    Uint32 seconds = bg->timer.total_time / 1000;
    snprintf(buf, sizeof(buf), "Final Time: %02d:%02d", seconds / 60, seconds % 60);
    SDL_Color yellow = {255, 255, 0};
    SDL_Surface *final_surface = TTF_RenderText_Solid(bg->timer.font, buf, yellow);
    SDL_Rect pos = {SCREEN_WIDTH / 2 - final_surface->w / 2, SCREEN_HEIGHT / 2 - final_surface->h / 2};
    SDL_BlitSurface(final_surface, NULL, screen, &pos);
    SDL_FreeSurface(final_surface);
}

static void load_texts(Background *bg) {
    SDL_Color white = {255, 255, 255};
    const char *msgs[3] = {
        "Press SPACE to jump",
        "Fight the enemy to proceed",
        "Congratulations! Press X for level 2"
    };
    int xs[3] = {650, 3700, 4500};
    for (int i = 0; i < 3; i++) {
        strcpy(bg->texts[i].text, msgs[i]);
        bg->texts[i].surface = TTF_RenderText_Solid(bg->timer.font, msgs[i], white);
        bg->texts[i].position = (SDL_Rect){xs[i], 100, 0, 0};
        bg->texts[i].active = 1;
        bg->texts[i].is_showing = 0;
    }
}

static void check_texts(Background *bg) {
    for (int i = 0; i < 3; i++) {
        if (!bg->texts[i].active || bg->texts[i].is_showing) continue;
        int tx = bg->texts[i].position.x - bg->camera.x;
        if (tx >= 0 && tx < SCREEN_WIDTH) {
            bg->texts[i].is_showing = 1;
            bg->texts[i].show_time = SDL_GetTicks();
        }
    }
}

static void draw_texts(Background *bg, SDL_Surface *screen) {
    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < 3; i++) {
        if (!bg->texts[i].active || !bg->texts[i].is_showing) continue;
        if (now - bg->texts[i].show_time <= TEXT_DISPLAY_TIME) {
            SDL_Rect pos = {
                bg->texts[i].position.x - bg->camera.x,
                bg->texts[i].position.y - bg->camera.y,
                0, 0
            };
            SDL_BlitSurface(bg->texts[i].surface, NULL, screen, &pos);
        } else {
            bg->texts[i].active = 0;
            SDL_FreeSurface(bg->texts[i].surface);
        }
    }
}

static void add_platform(Background *bg, const char *img, int x, int y, PlatformType t, int mr) {
    bg->platform_count++;
    bg->platforms = realloc(bg->platforms, bg->platform_count * sizeof(Platform));
    Platform *p = &bg->platforms[bg->platform_count - 1];
    p->image = IMG_Load(img);
    if (!p->image) {
        fprintf(stderr, "Error loading %s: %s\n", img, IMG_GetError());
        exit(EXIT_FAILURE);
    }
    p->position = (SDL_Rect){x, y, p->image->w, p->image->h};
    p->type = t;
    p->move_range = mr;
    p->move_direction = 1;
    p->original_x = x;
    p->original_y = y;
}

void init_background(Background *bg, int level, Uint32 start_ticks) {
    bg->level = level;
    bg->show_final_time = 0;

    const char *f = (level == 1) ? "bg4.png" : "bg2.jpg";
    bg->bg_image = IMG_Load(f);
    if (!bg->bg_image) {
        fprintf(stderr, "Error loading %s: %s\n", f, IMG_GetError());
        exit(EXIT_FAILURE);
    }

    bg->camera.x = 0;
    bg->camera.y = (level == 1) ? INITIAL_CAMERA_Y : 50;
    bg->camera.w = SCREEN_WIDTH;
    bg->camera.h = SCREEN_HEIGHT;

    bg->max_w = 5000;
    bg->max_h = 3000;

    bg->hole_img = IMG_Load("hole.png");
    bg->enemy_img = IMG_Load((level == 1) ? "enemy.png" : "enemy2.png");
    if (!bg->hole_img || !bg->enemy_img) {
        fprintf(stderr, "Error loading hole/enemy images\n");
        exit(EXIT_FAILURE);
    }

    bg->platforms = NULL;
    bg->platform_count = 0;
    init_timer(&bg->timer, start_ticks);

    if (level == 1) load_texts(bg);
    else for (int i = 0; i < 3; i++) bg->texts[i].active = 0;

    if (level == 1) {
        add_platform(bg, "plat.png",   800, 375, PLATFORM_FIXED, 0);
        add_platform(bg, "plat.png",  3000, 375, PLATFORM_FIXED, 0);
        add_platform(bg, "plat.png",  1900, 375, PLATFORM_MOBILE_H, 200);
        add_platform(bg, "plat0.png", 4500, 150, PLATFORM_FIXED, 0);
        add_platform(bg, "hole.png",   850, 340, ITEM_HOLE, 0);
        add_platform(bg, "hole.png",  1950, 340, ITEM_HOLE, 0);
        add_platform(bg, "hole.png",  3050, 340, ITEM_HOLE, 0);
        add_platform(bg, "enemy.png", 3800, 225, ENEMY, 0);
    } else {
        add_platform(bg, "plat2.png",   800, 480, PLATFORM_FIXED, 0);  // Manquante
        add_platform(bg, "plat2.png",  3000, 480, PLATFORM_FIXED, 0);
        add_platform(bg, "plat2.png",  1900, 480, PLATFORM_MOBILE_H, 200);
        add_platform(bg, "plat0.png",  4500, 150, PLATFORM_FIXED, 0);
        add_platform(bg, "hole.png",    850, 450, ITEM_HOLE, 0);
        add_platform(bg, "hole.png",   1950, 450, ITEM_HOLE, 0);
        add_platform(bg, "hole.png",   3050, 450, ITEM_HOLE, 0);
        add_platform(bg, "enemy2.png", 3800, 300, ENEMY, 0);
    }
}

void free_background(Background *bg) {
    SDL_FreeSurface(bg->bg_image);
    SDL_FreeSurface(bg->hole_img);
    SDL_FreeSurface(bg->enemy_img);
    for (int i = 0; i < bg->platform_count; i++) {
        SDL_FreeSurface(bg->platforms[i].image);
    }
    free(bg->platforms);
    for (int i = 0; i < 3; i++) {
        if (bg->texts[i].active) SDL_FreeSurface(bg->texts[i].surface);
    }
    if (bg->timer.time_surface) SDL_FreeSurface(bg->timer.time_surface);
    TTF_CloseFont(bg->timer.font);
    TTF_Quit();
}

void update_platforms(Background *bg) {
    for (int i = 0; i < bg->platform_count; i++) {
        Platform *p = &bg->platforms[i];
        if (p->type == PLATFORM_MOBILE_H) {
            p->position.x += MOBILE_PLATFORM_SPEED * p->move_direction;
            if (abs(p->position.x - p->original_x) >= p->move_range)
                p->move_direction *= -1;
        }
    }
}

void update_scrolling(Background *bg, Uint8 *keystate) {
    if (keystate[SDLK_RIGHT] && bg->camera.x < bg->max_w - SCREEN_WIDTH)
        bg->camera.x += SCROLL_SPEED;
    if (keystate[SDLK_LEFT] && bg->camera.x > 0)
        bg->camera.x -= SCROLL_SPEED;
    if (keystate[SDLK_DOWN] && bg->camera.y < bg->max_h - SCREEN_HEIGHT)
        bg->camera.y += SCROLL_SPEED;
    if (keystate[SDLK_UP] && bg->camera.y > 0)
        bg->camera.y -= SCROLL_SPEED;
}

void render_all(Background *bg, SDL_Surface *screen) {
    int w = bg->bg_image->w;
    int repeats = (SCREEN_WIDTH / w) + 2;
    for (int i = 0; i < repeats; i++) {
        SDL_Rect dst = {
            i * w - (bg->camera.x % w),
            -bg->camera.y,
            0, 0
        };
        SDL_BlitSurface(bg->bg_image, NULL, screen, &dst);
    }

    for (int i = 0; i < bg->platform_count; i++) {
        Platform *p = &bg->platforms[i];
        SDL_Rect dst = {
            p->position.x - bg->camera.x,
            p->position.y - bg->camera.y,
            0, 0
        };
        SDL_BlitSurface(p->image, NULL, screen, &dst);
    }

    check_texts(bg);
    draw_texts(bg, screen);

    update_timer(&bg->timer);
    if (bg->timer.time_surface && !bg->show_final_time) {
        SDL_Rect tdst = {10, 10, 0, 0};
        SDL_BlitSurface(bg->timer.time_surface, NULL, screen, &tdst);
    }

    if (bg->show_final_time) draw_final_time(bg, screen);
}

