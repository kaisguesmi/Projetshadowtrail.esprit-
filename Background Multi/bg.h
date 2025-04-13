#ifndef BG_H
#define BG_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_WIDTH   1800
#define SCREEN_HEIGHT   600
#define VIEW_W         (SCREEN_WIDTH/2)
#define VIEW_H         (SCREEN_HEIGHT)
#define WORLD_W        5000
#define WORLD_H        3000
#define SCROLL_SPEED      5
#define PLATFORM_SPEED    2
#define FONT_SIZE        24
#define TEXT_TIME      5000

typedef struct {
    SDL_Surface *img;
    int x, y;
    int type;
    int range, dir, orig_x;
} Platform;

typedef struct {
    SDL_Surface *surf;
    int x, y;
    int active;
    Uint32 start;
} TextMsg;

typedef struct {
    SDL_Surface *bg;
    SDL_Rect camera;
    Platform *plats;
    int n_plats;
    TextMsg texts[3];
    TTF_Font *font;
    SDL_Surface *timer_surf;
    Uint32 start_ticks;
    Uint32 total_time;
} Background;

void bg_init(Background *b, int level, int screen_id);
void bg_free(Background *b);
void bg_update(Background *b, const Uint8 *keys, int up, int down, int left, int right);
void bg_render(Background *b, SDL_Surface *screen, int vx);
int bg_get_camera_x(Background *b);

#endif // BG_Hs
