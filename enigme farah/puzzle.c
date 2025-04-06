#include "puzzle.h"
#include <stdlib.h>
#include <time.h>
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
void puzzle_init(PuzzleGame* game, const char* image_path) {
 game->text_color = (SDL_Color){255, 255, 255, 255};
    // Initialize game state
    game->state = PUZZLE_STATE_RUNNING;
    game->moves = 0;
    game->empty_pos = PUZZLE_SIZE * PUZZLE_SIZE - 1;
    game->remaining_time = TIMER_DURATION * 1000;
    game->start_time = SDL_GetTicks();
    game->font = NULL;
    game->background = NULL;
    game->hover_piece = -1;

    // Load image
    game->original_img = IMG_Load(image_path);
    if (!game->original_img) {
        game->original_img = SDL_CreateRGBSurface(SDL_SWSURFACE, 600, 600, 32, 0, 0, 0, 0);
        SDL_FillRect(game->original_img, NULL, SDL_MapRGB(game->original_img->format, 128, 128, 128));
    }

    // Create puzzle pieces
    int pw = game->original_img->w / PUZZLE_SIZE;
    int ph = game->original_img->h / PUZZLE_SIZE;
    SDL_Rect src;

    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (i == game->empty_pos) {
            game->pieces[i] = NULL;
            continue;
        }

        game->pieces[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, pw, ph, 32, 0, 0, 0, 0);
        src.x = (i % PUZZLE_SIZE) * pw;
        src.y = (i / PUZZLE_SIZE) * ph;
        src.w = pw;
        src.h = ph;
        SDL_BlitSurface(game->original_img, &src, game->pieces[i], NULL);

        game->positions[i].x = (i % PUZZLE_SIZE) * pw + 100;
        game->positions[i].y = (i / PUZZLE_SIZE) * ph + 100;
        game->positions[i].w = pw;
        game->positions[i].h = ph;
    }

    puzzle_shuffle(game);
}
void puzzle_load_background(PuzzleGame* game, const char* bg_path) {
    game->background = IMG_Load(bg_path);
    if (!game->background) {
        // Create fallback background
        game->background = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                             SCREEN_WIDTH, 
                                             SCREEN_HEIGHT, 
                                             32, 
                                             0, 0, 0, 0);
        // Make a gradient background
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            Uint32 color = SDL_MapRGB(game->background->format, 
                                    128 + y/4, 
                                    128 + y/4, 
                                    255 - y/8);
            SDL_Rect line = {0, y, SCREEN_WIDTH, 1};
            SDL_FillRect(game->background, &line, color);
        }
    }
    game->bg_rect = (SDL_Rect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
}

void puzzle_load_font(PuzzleGame* game, const char* font_path, int size) {
    game->font = TTF_OpenFont(font_path, size);
}

void puzzle_shuffle(PuzzleGame* game) {
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        int dir = rand() % 4;
        int new_pos = -1;
        int row = game->empty_pos / PUZZLE_SIZE;
        int col = game->empty_pos % PUZZLE_SIZE;

        switch (dir) {
            case 0: if (row > 0) new_pos = game->empty_pos - PUZZLE_SIZE; break;
            case 1: if (row < PUZZLE_SIZE-1) new_pos = game->empty_pos + PUZZLE_SIZE; break;
            case 2: if (col > 0) new_pos = game->empty_pos - 1; break;
            case 3: if (col < PUZZLE_SIZE-1) new_pos = game->empty_pos + 1; break;
        }

        if (new_pos != -1) {
            game->positions[game->empty_pos] = game->positions[new_pos];
            game->empty_pos = new_pos;
        }
    }
}

void puzzle_handle_event(PuzzleGame* game, SDL_Event* event) {
game->hover_piece = -1;  // Réinitialiser à chaque frame

    if (event->type == SDL_MOUSEMOTION) {
        int x = event->motion.x;
        int y = event->motion.y;

        for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
            if (i != game->empty_pos && 
                x >= game->positions[i].x && x < game->positions[i].x + game->positions[i].w &&
                y >= game->positions[i].y && y < game->positions[i].y + game->positions[i].h) {
                game->hover_piece = i;  // Mémoriser la pièce survolée
                break;
            }
        }
    }
    if (game->state != PUZZLE_STATE_RUNNING) return;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;

        for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
            if (i == game->empty_pos) continue;

            if (x >= game->positions[i].x && x < game->positions[i].x + game->positions[i].w &&
                y >= game->positions[i].y && y < game->positions[i].y + game->positions[i].h) {

                int row_diff = abs((i / PUZZLE_SIZE) - (game->empty_pos / PUZZLE_SIZE));
                int col_diff = abs((i % PUZZLE_SIZE) - (game->empty_pos % PUZZLE_SIZE));

                if ((row_diff == 1 && col_diff == 0) || (row_diff == 0 && col_diff == 1)) {
                    game->positions[game->empty_pos] = game->positions[i];
                    game->empty_pos = i;
                    game->moves++;

                    if (puzzle_check_win(game)) {
                        game->state = PUZZLE_STATE_WON;
                    }
                }
            }
        }
    }
}

void puzzle_update(PuzzleGame* game) {
    if (game->state != PUZZLE_STATE_RUNNING) return;

    Uint32 current = SDL_GetTicks();
    Uint32 elapsed = current - game->start_time;
    game->start_time = current;

    if (game->remaining_time > elapsed) {
        game->remaining_time -= elapsed;
    } else {
        game->remaining_time = 0;
        game->state = PUZZLE_STATE_LOST;
    }
}

int puzzle_check_win(PuzzleGame* game) {
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (i == game->empty_pos) continue;

        int correct_x = (i % PUZZLE_SIZE) * game->positions[i].w + 100;
        int correct_y = (i / PUZZLE_SIZE) * game->positions[i].h + 100;

        if (game->positions[i].x != correct_x || game->positions[i].y != correct_y) {
            return 0;
        }
    }
    return 1;
}
void puzzle_render(PuzzleGame* game, SDL_Surface* screen) {
    // Draw background
    SDL_BlitSurface(game->background, NULL, screen, &game->bg_rect);
    
    // Draw puzzle pieces
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (i != game->empty_pos && game->pieces[i]) {
        // Ajouter une bordure blanche autour de la pièce
            SDL_Rect border = {
                game->positions[i].x - 2,  // Décale de 2px à gauche
                game->positions[i].y - 2,  // Décale de 2px vers le haut
                game->positions[i].w + 4,   // Agrandit la largeur
                game->positions[i].h + 4    // Agrandit la hauteur
            };
            SDL_FillRect(screen, &border, SDL_MapRGB(screen->format, 255, 255, 255)); // Blanc
             if (i == game->hover_piece) {
                SDL_Rect highlight = {
                    game->positions[i].x - 4,
                    game->positions[i].y - 4,
                    game->positions[i].w + 8,
                    game->positions[i].h + 8
                };
                SDL_FillRect(screen, &highlight, SDL_MapRGB(screen->format, 255, 255, 0)); // Jaune

            SDL_BlitSurface(game->pieces[i], NULL, screen, &game->positions[i]);
        }
    }

    // Add a semi-transparent panel behind UI
    SDL_Surface* panel = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 80, 32, 0, 0, 0, 0);
    SDL_FillRect(panel, NULL, SDL_MapRGBA(panel->format, 0, 0, 0, 128));
    SDL_Rect panel_pos = {5, 5, 0, 0};
    SDL_BlitSurface(panel, NULL, screen, &panel_pos);
    SDL_FreeSurface(panel);

    puzzle_render_timer(game, screen);
    puzzle_render_moves(game, screen);

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (i != game->empty_pos && game->pieces[i]) {
            SDL_BlitSurface(game->pieces[i], NULL, screen, &game->positions[i]);
        }
    }

    puzzle_render_timer(game, screen);
    puzzle_render_moves(game, screen);

    if (game->state == PUZZLE_STATE_WON) {
        SDL_Surface* win_msg = TTF_RenderText_Solid(game->font, "You Won!", (SDL_Color){0, 255, 0, 255});
        SDL_Rect pos = {SCREEN_WIDTH/2 - win_msg->w/2, SCREEN_HEIGHT/2 - win_msg->h/2, 0, 0};
        SDL_BlitSurface(win_msg, NULL, screen, &pos);
        SDL_FreeSurface(win_msg);
    } else if (game->state == PUZZLE_STATE_LOST) {
        SDL_Surface* lose_msg = TTF_RenderText_Solid(game->font, "Time's Up!", (SDL_Color){255, 0, 0, 255});
        SDL_Rect pos = {SCREEN_WIDTH/2 - lose_msg->w/2, SCREEN_HEIGHT/2 - lose_msg->h/2, 0, 0};
        SDL_BlitSurface(lose_msg, NULL, screen, &pos);
        SDL_FreeSurface(lose_msg);
    }
}
}

void puzzle_render_timer(PuzzleGame* game, SDL_Surface* screen) {
    if (!game->font) return;

    int seconds = game->remaining_time / 1000;
    char timer_str[20];
    snprintf(timer_str, sizeof(timer_str), "Time: %02d:%02d", seconds/60, seconds%60);

    SDL_Surface* timer_surface = TTF_RenderText_Solid(game->font, timer_str, game->text_color);
    SDL_Rect pos = {10, 10, 0, 0};
    SDL_BlitSurface(timer_surface, NULL, screen, &pos);
    SDL_FreeSurface(timer_surface);
}

void puzzle_render_moves(PuzzleGame* game, SDL_Surface* screen) {
    if (!game->font) return;

    char moves_str[20];
    snprintf(moves_str, sizeof(moves_str), "Moves: %d", game->moves);

    SDL_Surface* moves_surface = TTF_RenderText_Solid(game->font, moves_str, game->text_color);
    SDL_Rect pos = {10, 40, 0, 0};
    SDL_BlitSurface(moves_surface, NULL, screen, &pos);
    SDL_FreeSurface(moves_surface);
}

void puzzle_cleanup(PuzzleGame* game) {
if (game->background) {
        SDL_FreeSurface(game->background);
    }
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (game->pieces[i]) {
            SDL_FreeSurface(game->pieces[i]);
        }
    }
    if (game->original_img) {
        SDL_FreeSurface(game->original_img);
    }
    if (game->font) {
        TTF_CloseFont(game->font);
    }
}


