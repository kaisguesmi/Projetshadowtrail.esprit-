// main.c
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "perso.h"
#include <stdio.h>
#include <stdlib.h>

// Rectangle obstacle de test
SDL_Rect obstacle = {600, GROUND_Y - 50, 50, 50}; // Position obstacle relative to ground

// Simple AABB collision check
int intersects(SDL_Rect a, SDL_Rect b)
{
    if (a.x + a.w <= b.x) return 0; if (a.x >= b.x + b.w) return 0;
    if (a.y + a.h <= b.y) return 0; if (a.y >= b.y + b.h) return 0;
    return 1;
}

// Helper to get the current bounding box of the character
SDL_Rect getPersoBoundingBox(const personne* p) {
    SDL_Rect box = p->position;
    // Si mort, utiliser les dimensions de l'image de mort si elle existe
    if (p->is_dead && p->death_sprite) {
         box.w = p->death_sprite->w;
         box.h = p->death_sprite->h;
         return box;
    }
    // Sinon, logique normale pour perso vivant
    int current_action_index = p->direction; int current_frame_index = p->nbr_frame;
    if (p->is_attacking) { current_action_index = 2; }
    else if (p->up) { current_action_index = 3; }
    else if (p->vitesse > 0) {
        if (p->vitesse > RUN_SPEED_THRESHOLD) {
            current_action_index = (p->direction == 0) ? 4 : 5;
            if (current_action_index == 5 && p->images[5][0] == NULL) { current_action_index = 1; }
        } else { current_action_index = p->direction; }
    } else { current_frame_index = IDLE_FRAME_INDEX; }
    SDL_Surface* current_sprite = NULL;
    if (current_action_index >= 0 && current_action_index < NB_ACTIONS && current_frame_index >= 0 && current_frame_index < NB_FRAMES) {
        current_sprite = p->images[current_action_index][current_frame_index];
    }
    if (current_sprite) { box.w = current_sprite->w; box.h = current_sprite->h; }
    else if (p->images[0][0]) { box.w = p->images[0][0]->w; box.h = p->images[0][0]->h; }
    else { box.w = 30; box.h = 50; }
    return box;
}

// --- Helper function to display centered text ---
void displayText(const char* message, SDL_Surface* screen, TTF_Font* font, SDL_Color color) {
    if (!font || !screen || !message) return;
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, message, color); // Blended est plus joli
    if (textSurface) {
        SDL_Rect textLocation;
        textLocation.x = (SCREEN_W - textSurface->w) / 2;
        textLocation.y = (SCREEN_H - textSurface->h) / 2;
        textLocation.w = textSurface->w; textLocation.h = textSurface->h;
        SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
        SDL_FreeSurface(textSurface);
    } else { fprintf(stderr, "TTF_RenderText Error: %s\n", TTF_GetError()); }
}


int main(int argc, char *argv[])
{
    // --- Initialization ---
    printf("Initializing SDL...\n"); if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError()); return 1; }
    printf("Initializing SDL_image...\n"); if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError()); SDL_Quit(); return 1; }
    printf("Initializing SDL_ttf...\n"); if (TTF_Init() == -1) { fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError()); IMG_Quit(); SDL_Quit(); return 1; }
    printf("Creating screen surface...\n"); SDL_Surface *screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) { fprintf(stderr, "SDL_SetVideoMode Error: %s\n", SDL_GetError()); TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1; }
    SDL_WM_SetCaption("SDL Personnage Death Test", NULL);

    // --- Load Assets ---
    printf("Loading assets...\n"); SDL_Surface *back = IMG_Load("background.png"); SDL_Surface *obs_img = IMG_Load("obstacle.png");
    // IMPORTANT: Utiliser une police et ajuster la taille si besoin
    const char* font_path = "arial.ttf";
    int font_size_ui = 24; // Taille pour score/vie
    int font_size_msg = 48; // Taille pour messages centraux
    printf("Loading font: %s (sizes %d, %d)\n", font_path, font_size_ui, font_size_msg);
    TTF_Font *font_ui = TTF_OpenFont(font_path, font_size_ui);
    TTF_Font *font_msg = TTF_OpenFont(font_path, font_size_msg);
    if (!font_ui || !font_msg) { fprintf(stderr, "TTF_OpenFont Error loading '%s': %s\n", font_path, TTF_GetError()); if(font_ui) TTF_CloseFont(font_ui); if(font_msg) TTF_CloseFont(font_msg); TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1; }
    if (!back || !obs_img) { fprintf(stderr, "Error loading background.png or obstacle.png: %s\n", IMG_GetError()); TTF_CloseFont(font_ui); TTF_CloseFont(font_msg); if(back) SDL_FreeSurface(back); if(obs_img) SDL_FreeSurface(obs_img); TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1; }
    printf("Assets loaded successfully.\n");

    // --- Initialize Players ---
    personne p, p1; printf("Initializing player 1...\n"); initialiserperso(&p); printf("Initializing player 2...\n"); initialiserperso2(&p1); printf("Initialization complete.\n");

    // --- Game Loop Variables ---
    SDL_Event event; int running = 1; Uint32 t_prev = SDL_GetTicks(); const float base_speed = 200.0;
    int p1_move_left = 0, p1_move_right = 0; int p2_move_left = 0, p2_move_right = 0;
    int game_over = 0; // Flag pour l'état Game Over
    SDL_Color message_color = {255, 200, 0, 0}; // Couleur pour les messages

    printf("Starting main game loop...\n");
    // --- Main Game Loop ---
    while (running) {
        Uint32 now = SDL_GetTicks(); Uint32 dt = (now > t_prev) ? (now - t_prev) : 1; t_prev = now;

        // --- Event Handling ---
        if (!game_over) { // Gérer events seulement si pas Game Over
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) { running = 0; }
                else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_RIGHT: if(!p.is_dead) { p1_move_right = 1; p.direction = 0; } break;
                        case SDLK_LEFT:  if(!p.is_dead) { p1_move_left = 1;  p.direction = 1; } break;
                        case SDLK_UP:    if (!p.is_dead && !p.up && !p.is_attacking && p.vitesse_saut == 0) { p.up = 1; } break;
                        case SDLK_SPACE: if(!p.is_dead) startAttack(&p, now); break;
                        case SDLK_d:     if(!p1.is_dead) { p2_move_right = 1; p1.direction = 0; } break;
                        case SDLK_q:     if(!p1.is_dead) { p2_move_left = 1;  p1.direction = 1; } break;
                        case SDLK_z:     if (!p1.is_dead && !p1.up && !p1.is_attacking && p1.vitesse_saut == 0) { p1.up = 1; } break;
                        case SDLK_e:     if(!p1.is_dead) startAttack(&p1, now); break;
                        case SDLK_ESCAPE: running = 0; break;
                    }
                } else if (event.type == SDL_KEYUP) {
                    switch (event.key.keysym.sym) {
                        case SDLK_RIGHT: p1_move_right = 0; break; case SDLK_LEFT:  p1_move_left = 0;  break;
                        case SDLK_d: p2_move_right = 0; break; case SDLK_q: p2_move_left = 0; break;
                    }
                }
            }
        } else { // Si Game Over, gérer seulement Quit/Escape
             while (SDL_PollEvent(&event)) {
                 if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) { running = 0; }
             }
        }

        // --- Update Movement Speed ---
        if (!game_over) {
            p.vitesse = (!p.is_dead && p1_move_right && !p1_move_left) ? base_speed : (!p.is_dead && p1_move_left && !p1_move_right ? base_speed : 0); if(p.vitesse > 0) p.direction = p1_move_right ? 0 : 1;
            p1.vitesse = (!p1.is_dead && p2_move_right && !p2_move_left) ? base_speed : (!p1.is_dead && p2_move_left && !p2_move_right ? base_speed : 0); if(p1.vitesse > 0) p1.direction = p2_move_right ? 0 : 1;
        } else { p.vitesse = 0; p1.vitesse = 0; } // Stopper si game over

        // --- Game Logic Updates ---
        // Les fonctions internes vérifient p->is_dead maintenant
        moveperso(&p, dt); if (p.up || p.vitesse_saut != 0) { saut(&p, GROUND_Y, dt); } updateAnimation(&p, now);
        moveperso(&p1, dt); if (p1.up || p1.vitesse_saut != 0) { saut(&p1, GROUND_Y, dt); } updateAnimation(&p1, now);

        // --- Collision Detection & Response ---
        if (!game_over) { // Pas de collision si game over
            SDL_Rect p_box = getPersoBoundingBox(&p); SDL_Rect p1_box = getPersoBoundingBox(&p1);
            if (!p.is_dead && intersects(p_box, obstacle)) { // Vérifier si vivant avant collision
                perdre_vie(&p);
                // Push back simplifié
                if (p.vitesse > 0 && !p.is_attacking) { if (p.direction == 0) { p.position.x = obstacle.x - p_box.w - 1; } else { p.position.x = obstacle.x + obstacle.w + 1; } p.vitesse = 0; }
                else if (p.up && p.vitesse_saut > 0 && (p_box.y + p_box.h) > obstacle.y && p_box.x < (obstacle.x + obstacle.w) && (p_box.x + p_box.w) > obstacle.x ) { p.position.y = obstacle.y - p_box.h; p.vitesse_saut = 0; p.up = 0; }
            }
            if (!p1.is_dead && intersects(p1_box, obstacle)) { // Vérifier si vivant avant collision
                 perdre_vie(&p1);
                 if (p1.vitesse > 0 && !p1.is_attacking) { if (p1.direction == 0) { p1.position.x = obstacle.x - p1_box.w - 1; } else { p1.position.x = obstacle.x + obstacle.w + 1; } p1.vitesse = 0; }
                 else if (p1.up && p1.vitesse_saut > 0 && (p1_box.y + p1_box.h) > obstacle.y && p1_box.x < (obstacle.x + obstacle.w) && (p1_box.x + p1_box.w) > obstacle.x) { p1.position.y = obstacle.y - p1_box.h; p1.vitesse_saut = 0; p1.up = 0; }
            }
        }

        // --- Vérifier Game Over ---
        if (!game_over && p.is_dead && p1.is_dead) {
            printf("GAME OVER!\n");
            game_over = 1;
        }

        // --- Rendering ---
        SDL_BlitSurface(back, NULL, screen, NULL); SDL_BlitSurface(obs_img, NULL, screen, &obstacle);
        // P1
        afficherperso(p, screen); // Affiche le sprite de mort si p.is_dead
        affichervie(p, screen, 10, 10);
        afficherScore(p, screen, font_ui, 150, 10); // Utiliser font_ui
        // P2
        afficherperso(p1, screen); // Affiche le sprite de mort si p1.is_dead
        affichervie(p1, screen, 10, 70);
        afficherScore(p1, screen, font_ui, 150, 70); // Utiliser font_ui

        // --- Afficher les messages par-dessus ---
        if (game_over) {
            displayText("Game Over", screen, font_msg, message_color); // Utiliser font_msg
        } else if (p.is_dead != p1.is_dead) { // Exactement un joueur mort (XOR)
            displayText("Vous avez perdu un perso", screen, font_msg, message_color); // Utiliser font_msg
        }

        SDL_Flip(screen);

        // --- Frame Limiting ---
        Uint32 frame_time = SDL_GetTicks() - now; if (frame_time < 16) { SDL_Delay(16 - frame_time); }
    } // --- End Game Loop ---

    // --- Cleanup ---
    printf("Cleaning up resources...\n"); freePerso(&p); freePerso(&p1); SDL_FreeSurface(back); SDL_FreeSurface(obs_img);
    TTF_CloseFont(font_ui); TTF_CloseFont(font_msg); // Fermer les deux polices
    printf("Quitting SDL subsystems...\n"); TTF_Quit(); IMG_Quit(); SDL_Quit(); printf("Exiting program.\n");
    return 0;
}
