#include "puzzle.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void puzzle_init(PuzzleGame* game, const char* image_paths[]) {
    // Initialisation de base
    game->state = PUZZLE_STATE_RUNNING;
    game->moves = 0;
    game->empty_pos = PUZZLE_SIZE * PUZZLE_SIZE - 1;
    game->remaining_time = TIMER_DURATION * 1000;
    game->start_time = SDL_GetTicks();
    game->font = NULL;
    game->text_color = (SDL_Color){0,0, 0, 255};
    game->hover_piece = -1;
    game->piece_selected = false;
    game->selected_piece = -1;
     game->need_redraw = true; 
      for(int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        game->piece_locked[i] = false;
    }
    game->background = IMG_Load("background.jpg");
    if (!game->background) {
        printf("Erreur chargement background: %s\n", IMG_GetError());
        // Vous pouvez choisir de quitter ou de continuer sans fond
    }
      // Charger la musique
    game->background_music = Mix_LoadMUS("musique.mp3");
    if(!game->background_music) {
        printf("Failed to load music: %s\n", Mix_GetError());
    }

    // Jouer la musique en boucle
    if(game->background_music) {
        Mix_PlayMusic(game->background_music, -1); // -1 pour boucler
    }
    // Chargement des pièces individuelles
    int piece_width = 0, piece_height = 0;
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        game->pieces[i] = IMG_Load(image_paths[i]);
        if (!game->pieces[i]) {
            printf("Failed to load piece %d\n", i);
            exit(1);
        }

        if (i == 0) {
            piece_width = game->pieces[i]->w;
            piece_height = game->pieces[i]->h;
        }

        // Positions correctes
        game->correct_positions[i].x = (i % PUZZLE_SIZE) * piece_width + 100;
        game->correct_positions[i].y = (i / PUZZLE_SIZE) * piece_height + 160;
        game->correct_positions[i].w = piece_width;
        game->correct_positions[i].h = piece_height;
    }

    // Positions initiales (mélangées)
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        game->positions[i] = game->correct_positions[i];
    }

    // Mélanger
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
     // Charger la sprite sheet du chrono
    game->timer_sheet = IMG_Load("chronometre.png");
    if (!game->timer_sheet) {
        printf("Failed to load timer sheet!\n");
        exit(1);
    }

    // Initialiser les cadrans (100x100 px chacun)
    int frame_width = 817/6;
int frame_height = 305 / 2; // = 152 ou 152.5

for (int i = 0; i < 12; i++) {
    int col = i % 6;             // 0 à 5
    int row = i / 6;             // 0 ou 1
    game->timer_frames[i] = (SDL_Rect){col * frame_width, row * frame_height, frame_width, frame_height};
}
    game->current_timer_frame = 0;
}
void puzzle_load_font(PuzzleGame* game, const char* font_path, int size) {
    game->font = TTF_OpenFont(font_path, size);
}
bool is_valid_move(PuzzleGame* game, int piece_index) {
    int empty_row = game->empty_pos / PUZZLE_SIZE;
    int empty_col = game->empty_pos % PUZZLE_SIZE;
    int piece_row = piece_index / PUZZLE_SIZE;
    int piece_col = piece_index % PUZZLE_SIZE;
    
    return (abs(empty_row - piece_row) == 1 && empty_col == piece_col)||
           (abs(empty_col - piece_col) == 1 && empty_row == piece_row);
}

void puzzle_handle_event(PuzzleGame* game, SDL_Event* event) {
    game->hover_piece = -1;

    if (game->state != PUZZLE_STATE_RUNNING) return;

    // Détection du survol
    if (event->type == SDL_MOUSEMOTION) {
        int x = event->motion.x;
        int y = event->motion.y;

        for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
            if (i != game->empty_pos && 
                x >= game->positions[i].x && x < game->positions[i].x + game->positions[i].w &&
                y >= game->positions[i].y && y < game->positions[i].y + game->positions[i].h) {
                game->hover_piece = i;
                break;
            }
        }

        // Déplacement pendant le drag
        if (game->selected_piece != -1) {
            game->positions[game->selected_piece].x = event->motion.x - game->drag_offset.x;
            game->positions[game->selected_piece].y = event->motion.y - game->drag_offset.y;
        }
    }

    // Sélection de la pièce
     if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;

        for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
            if (i != game->empty_pos && 
                x >= game->positions[i].x && x < game->positions[i].x + game->positions[i].w &&
                y >= game->positions[i].y && y < game->positions[i].y + game->positions[i].h) {
                
                game->selected_piece = i;
                game->drag_offset.x = x - game->positions[i].x;
                game->drag_offset.y = y - game->positions[i].y;
                break;
            }
        }
        game->need_redraw = true;
    }

    // Relâchement de la pièce
    if(event->type == SDL_MOUSEBUTTONUP && game->selected_piece != -1) {
        int i = game->selected_piece;
        game->selected_piece = -1;

        if(is_valid_move(game, i)) {
            // Échange des positions
            SDL_Rect temp = game->positions[i];
            game->positions[i] = game->positions[game->empty_pos];
            game->positions[game->empty_pos] = temp;
            game->empty_pos = i;
            game->moves++;
            

            // Vérification et verrouillage POUR TOUTES LES PIÈCES
            for(int j = 0; j < PUZZLE_SIZE * PUZZLE_SIZE; j++) {
                if(j != game->empty_pos) {
                    game->piece_locked[j] = (game->positions[j].x == game->correct_positions[j].x &&
                                           game->positions[j].y == game->correct_positions[j].y);
                }
            }
           
        } else {
            game->positions[i] = game->correct_positions[i];
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
    // Changer de frame toutes les 5 secondes
    int seconds_left = game->remaining_time / 1000;
    game->current_timer_frame = (60 - seconds_left) / 5;
    if (game->current_timer_frame > 11) game->current_timer_frame = 11;

    // Clignotement rouge (dernières 10s)
    if (seconds_left < 10 && (current / 200) % 2) {
        // Animation warning (à implémenter dans le rendu)
    }
}

void puzzle_render(PuzzleGame* game, SDL_Surface* screen) {
if (game->background) {
        SDL_BlitSurface(game->background, NULL, screen, NULL);
    } else {
        // Fond de secours si l'image n'est pas chargée
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 200, 200, 255));
    }

    // Dessiner les pièces
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (i != game->empty_pos && game->pieces[i]) {
            // Bordure de survol
            if (i == game->hover_piece) {
                SDL_Rect highlight = {
                    game->positions[i].x - 4,
                    game->positions[i].y - 4,
                    game->positions[i].w + 8,
                    game->positions[i].h + 8
                };
                SDL_FillRect(screen, &highlight, SDL_MapRGB(screen->format, 255, 255, 0));
            }

            // Dessiner la pièce
            SDL_BlitSurface(game->pieces[i], NULL, screen, &game->positions[i]);
        }
    }

    // UI
    if (game->font) {
    
        // Timer
        int seconds = game->remaining_time / 1000;
        char timer_str[20];
        snprintf(timer_str, sizeof(timer_str), "Time: %02d:%02d", seconds/60, seconds%60);
        SDL_Surface* timer_surface = TTF_RenderText_Solid(game->font, timer_str, game->text_color);
        SDL_Rect timer_pos = {10, 10, 0, 0};
        SDL_BlitSurface(timer_surface, NULL, screen, &timer_pos);
        SDL_FreeSurface(timer_surface);

        // Moves
        char moves_str[20];
        snprintf(moves_str, sizeof(moves_str), "Moves: %d", game->moves);
        SDL_Surface* moves_surface = TTF_RenderText_Solid(game->font, moves_str, game->text_color);
        SDL_Rect moves_pos = {10, 50, 0, 0};
        SDL_BlitSurface(moves_surface, NULL, screen, &moves_pos);
        SDL_FreeSurface(moves_surface);
    }

    // Messages de fin
    if (game->state == PUZZLE_STATE_WON) {
        SDL_Surface* win_msg = TTF_RenderText_Solid(game->font, "You Won!", (SDL_Color){0, 255, 0, 255});
        SDL_Rect pos = {
        screen->w/2 - win_msg->w/2,  // Centre horizontal
        20,                           // 20 pixels du haut
        0, 
        0
    };
        SDL_BlitSurface(win_msg, NULL,screen , &pos);
        SDL_FreeSurface(win_msg);
    } 
    else if (game->state == PUZZLE_STATE_LOST) {
    SDL_Surface* lose_msg = TTF_RenderText_Solid(game->font, "Time's Up!", (SDL_Color){255, 0, 0, 255});
    SDL_Rect pos = {
        screen->w/2 - lose_msg->w/2,  // Centre horizontal
        20,                           // 20 pixels du haut
        0, 
        0
    };
    SDL_BlitSurface(lose_msg, NULL, screen, &pos);
    SDL_FreeSurface(lose_msg);
}
    // Afficher le chrono (en haut à droite)
    if (game->timer_sheet) {
        SDL_Rect timer_pos;
timer_pos.x = (screen->w - 100) / 2;  // centrer horizontalement (100 est la largeur du chrono)
timer_pos.y = 20;                     // 20 pixels depuis le haut
timer_pos.w = 100;
timer_pos.h = 100;
        
        // Dessiner le cadran approprié
        SDL_BlitSurface(game->timer_sheet, 
                       &game->timer_frames[game->current_timer_frame], 
                       screen, 
                       &timer_pos);

        // Animation warning
        if ((game->remaining_time / 1000) < 10 && (SDL_GetTicks() / 200) % 2) {
            SDL_Rect warn_rect = {timer_pos.x-5, timer_pos.y-5, 110, 110};
            SDL_FillRect(screen, &warn_rect, SDL_MapRGB(screen->format, 255, 0, 0));
        }
    }
     
    // Dessiner toutes les pièces SAUF la sélectionnée
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (i != game->empty_pos && i != game->selected_piece && game->pieces[i]) {
            SDL_BlitSurface(game->pieces[i], NULL, screen, &game->positions[i]);
        }
    }

    // Dessiner la pièce sélectionnée PAR-DESSUS
    if (game->selected_piece != -1) {
        SDL_BlitSurface(game->pieces[game->selected_piece], NULL, 
                       screen, &game->positions[game->selected_piece]);
    }	
    for(int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if(i != game->empty_pos && game->pieces[i]) {
            if(game->piece_locked[i]) {
                // Méthode sans SDL_image :
                SDL_Rect overlay = game->positions[i];
                SDL_FillRect(screen, &overlay, SDL_MapRGB(screen->format, 200, 200, 200));
            }
            SDL_BlitSurface(game->pieces[i], NULL, screen, &game->positions[i]);
        }
    }
}

void puzzle_cleanup(PuzzleGame* game) {
    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++) {
        if (game->pieces[i]) {
            SDL_FreeSurface(game->pieces[i]);
        }
    }
    if (game->background) {
        SDL_FreeSurface(game->background);
    }
    
    if (game->font) {
        TTF_CloseFont(game->font);
    }
     if (game->timer_sheet) SDL_FreeSurface(game->timer_sheet);
       
	if(game->background_music) {
        Mix_FreeMusic(game->background_music);
    }
    
}
