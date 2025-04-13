#ifndef BG_H
#define BG_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_WIDTH        1800
#define SCREEN_HEIGHT        600
#define INITIAL_CAMERA_Y       0
#define SCROLL_SPEED           5
#define MOBILE_PLATFORM_SPEED  2
#define TIMER_FONT_SIZE       24
#define TEXT_DISPLAY_TIME  5000

typedef enum {
    PLATFORM_FIXED,
    PLATFORM_MOBILE_H,
    ITEM_HOLE,
    ENEMY
} PlatformType;

typedef struct {
    SDL_Surface *image;
    SDL_Rect    position;
    PlatformType type;
    int         move_range;
    int         move_direction;
    int         original_x;
    int         original_y;
} Platform;

typedef struct {
    Uint32       start_ticks;
    TTF_Font    *font;
    SDL_Surface *time_surface;
    Uint32       total_time;
} GameTimer;

typedef struct {
    SDL_Surface *surface;
    SDL_Rect     position;
    char         text[128];
    int          active;
    int          is_showing;
    Uint32       show_time;
} GameText;

typedef struct {
    SDL_Surface *bg_image;
    SDL_Rect     camera;
    int          max_w;
    int          max_h;

    Platform    *platforms;
    int          platform_count;

    SDL_Surface *hole_img;
    SDL_Surface *enemy_img;

    GameTimer    timer;
    GameText     texts[3];

    int          level;
    int          show_final_time;
} Background;

void init_background(Background *bg, int level, Uint32 start_ticks);
void free_background(Background *bg);

void update_platforms(Background *bg);
void update_scrolling(Background *bg, Uint8 *keystate);
void update_timer(GameTimer *timer);

void render_all(Background *bg, SDL_Surface *screen);

#endif // BG_H

