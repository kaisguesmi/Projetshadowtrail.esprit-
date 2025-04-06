#ifndef PUZZLE_H
#define PUZZLE_H
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#define PUZZLE_SIZE 3
#define TIMER_DURATION 60 // 60 seconds

typedef enum {
    PUZZLE_STATE_RUNNING,
    PUZZLE_STATE_WON,
    PUZZLE_STATE_LOST
} PuzzleState;

typedef struct {
    SDL_Surface* pieces[PUZZLE_SIZE * PUZZLE_SIZE];
    SDL_Rect positions[PUZZLE_SIZE * PUZZLE_SIZE];
    int empty_pos;
    PuzzleState state;
    int moves;
    Uint32 start_time;
    Uint32 remaining_time;
    SDL_Surface* original_img;
    TTF_Font* font;
    SDL_Color text_color;
    int animating_piece;
    SDL_Rect target_position;
    Uint32 animation_start;
     SDL_Surface* background;  // Add background surface
    SDL_Rect bg_rect;         // Background position/size
     int hover_piece;
} PuzzleGame;

// Initialization
void puzzle_init(PuzzleGame* game, const char* image_path);
void puzzle_load_background(PuzzleGame* game, const char* bg_path) ;
void puzzle_load_font(PuzzleGame* game, const char* font_path, int size);

// Game logic
void puzzle_handle_event(PuzzleGame* game, SDL_Event* event);
void puzzle_update(PuzzleGame* game);
int puzzle_check_win(PuzzleGame* game);
void puzzle_shuffle(PuzzleGame* game);

// Rendering
void puzzle_render(PuzzleGame* game, SDL_Surface* screen);
void puzzle_render_timer(PuzzleGame* game, SDL_Surface* screen);
void puzzle_render_moves(PuzzleGame* game, SDL_Surface* screen);

// Cleanup
void puzzle_cleanup(PuzzleGame* game);

#endif
