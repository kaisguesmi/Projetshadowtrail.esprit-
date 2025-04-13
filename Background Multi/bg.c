#include "bg.h"
#include <stdlib.h>
#include <stdio.h>

static void init_timer(Background *b) {
    b->start_ticks = SDL_GetTicks();
    b->timer_surf = NULL;
    b->font = TTF_OpenFont("arial.ttf", FONT_SIZE);
    if (!b->font) { fprintf(stderr, "Font error: %s\n", TTF_GetError()); exit(1); }
}

static void update_timer(Background *b) {
    Uint32 elapsed = SDL_GetTicks() - b->start_ticks;
    b->total_time = elapsed;
    int s = (elapsed / 1000) % 60;
    int m = (elapsed / 60000);
    char buf[32]; snprintf(buf, 32, "Time %02d:%02d", m, s);
    SDL_Color c = {255, 255, 255};
    if (b->timer_surf) SDL_FreeSurface(b->timer_surf);
    b->timer_surf = TTF_RenderText_Solid(b->font, buf, c);
}

static void load_world(Background *b, int level, int screen_id) {
    b->bg = IMG_Load(level == 1 ? "bg4.png" : "bg2.jpg");
    b->n_plats = 0;
    b->plats = NULL;

    int coords[][4] = {{800,375,0,0},{3000,375,0,0},{1900,375,1,200},{4500,150,0,0}};
    for (int i = 0; i < 4; i++) {
        b->plats = realloc(b->plats, ++b->n_plats * sizeof(Platform));
        Platform *p = &b->plats[b->n_plats - 1];
        p->img = IMG_Load(i < 3 ? "plat.png" : "plat0.png");
        p->x = coords[i][0]; p->y = coords[i][1];
        p->type = coords[i][2]; p->range = coords[i][3];
        p->dir = 1; p->orig_x = p->x;
    }

    int holes[][2] = {{850,340},{1950,340},{3050,340}};
    for (int i = 0; i < 3; i++) {
        b->plats = realloc(b->plats, ++b->n_plats * sizeof(Platform));
        Platform *p = &b->plats[b->n_plats - 1];
        p->img = IMG_Load("hole.png");
        p->x = holes[i][0]; p->y = holes[i][1];
        p->type = 2; p->range = 0; p->dir = 0; p->orig_x = p->x;
    }

    b->plats = realloc(b->plats, ++b->n_plats * sizeof(Platform));
    Platform *e = &b->plats[b->n_plats - 1];
    e->img = IMG_Load(level == 1 ? "enemy.png" : "enemy2.png");
    e->x = 3800; e->y = (level == 1 ? 225 : 300); e->type = 3;

    if (level == 1) {
        const char *msgs[3];
        int xs[3] = {650, 3700, 4500};
        if (screen_id == 1) {
            msgs[0] = "Press SPACE to jump";
            msgs[1] = "Fight the enemy";
            msgs[2] = "Congratulations press X to proceed to level 2";
        } else {
            msgs[0] = "Press SPACE to jump";
            msgs[1] = "Fight the enemy";
            msgs[2] = "Congratulations press C to proceed to level 2";
        }

        SDL_Color white = {255,255,255};
        for (int i = 0; i < 3; i++) {
            b->texts[i].surf = TTF_RenderText_Solid(b->font, msgs[i], white);
            b->texts[i].x = xs[i]; b->texts[i].y = 100;
            b->texts[i].active = 1;
            b->texts[i].start = 0;
        }
    } else {
        for (int i = 0; i < 3; i++) b->texts[i].active = 0;
    }
}

void bg_init(Background *b, int level, int screen_id) {
    b->camera.x = 0; b->camera.y = 0;
    b->camera.w = VIEW_W; b->camera.h = VIEW_H;
    init_timer(b);
    load_world(b, level, screen_id);
}

void bg_free(Background *b) {
    SDL_FreeSurface(b->bg);
    SDL_FreeSurface(b->timer_surf);
    for (int i = 0; i < b->n_plats; i++) SDL_FreeSurface(b->plats[i].img);
    free(b->plats);
    for (int i = 0; i < 3; i++) SDL_FreeSurface(b->texts[i].surf);
    TTF_CloseFont(b->font);
}

void bg_update(Background *b, const Uint8 *keys, int up, int down, int left, int right) {
    if (keys[right] && b->camera.x < WORLD_W - VIEW_W) b->camera.x += SCROLL_SPEED;
    if (keys[left]  && b->camera.x > 0) b->camera.x -= SCROLL_SPEED;
    if (keys[down]  && b->camera.y < WORLD_H - VIEW_H) b->camera.y += SCROLL_SPEED;
    if (keys[up]    && b->camera.y > 0) b->camera.y -= SCROLL_SPEED;

    for (int i = 0; i < b->n_plats; i++) {
        Platform *p = &b->plats[i];
        if (p->type == 1) {
            p->x += PLATFORM_SPEED * p->dir;
            if (abs(p->x - p->orig_x) >= p->range) p->dir *= -1;
        }
    }

    update_timer(b);

    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < 3; i++) {
        if (b->texts[i].active && b->texts[i].start == 0) {
            int tx = b->texts[i].x - b->camera.x;
            if (tx >= 0 && tx < VIEW_W) b->texts[i].start = now;
        }
    }
}

void bg_render(Background *b, SDL_Surface *screen, int vx) {
    SDL_Rect old_clip;
    SDL_GetClipRect(screen, &old_clip);
    SDL_Rect clip = {vx, 0, VIEW_W, VIEW_H};
    SDL_SetClipRect(screen, &clip);

    int w = b->bg->w;
    for (int i = 0; i <= VIEW_W / w + 1; i++) {
        SDL_Rect dst = {vx + i*w - (b->camera.x % w), -b->camera.y, 0, 0};
        SDL_BlitSurface(b->bg, NULL, screen, &dst);
    }

    for (int i = 0; i < b->n_plats; i++) {
        SDL_Rect dst = {vx + b->plats[i].x - b->camera.x, b->plats[i].y - b->camera.y, 0, 0};
        SDL_BlitSurface(b->plats[i].img, NULL, screen, &dst);
    }

    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < 3; i++) {
        if (b->texts[i].start && now - b->texts[i].start < TEXT_TIME) {
            SDL_Rect dst = {vx + b->texts[i].x - b->camera.x, b->texts[i].y - b->camera.y, 0, 0};
            SDL_BlitSurface(b->texts[i].surf, NULL, screen, &dst);
        }
    }

    if (b->timer_surf) {
        SDL_Rect t = {vx + 10, 10, 0, 0};
        SDL_BlitSurface(b->timer_surf, NULL, screen, &t);
    }

    SDL_SetClipRect(screen, &old_clip);
}

int bg_get_camera_x(Background *b) {
    return b->camera.x;
}
