// main.c
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h> // Inclure SDL_mixer
#include "perso.h"
#include <stdio.h>
#include <stdlib.h>

// --- Global Variables / Constants ---
SDL_Rect obstacle = {600, GROUND_Y - 50, 50, 50};
Mix_Chunk *p1_jump_sound_collision = NULL; // Son saut P1 + Collision P1 (anciennement jump_sound)
Mix_Chunk *p1_death_sound = NULL;          // Son mort P1 (anciennement death_sound)
Mix_Chunk *p2_jump_sound = NULL;           // Son saut P2 (anciennement p1_jump_sound)
Mix_Chunk *p2_death_sound = NULL;          // Son mort P2


// --- Helper Functions ---
int intersects(SDL_Rect a, SDL_Rect b) { if (a.x + a.w <= b.x) return 0; if (a.x >= b.x + b.w) return 0; if (a.y + a.h <= b.y) return 0; if (a.y >= b.y + b.h) return 0; return 1; }

SDL_Rect getPersoBoundingBox(const personne* p) { // Pour collision au sol (simplifié)
    SDL_Rect box; box.x = p->position.x; box.y = p->position.y; SDL_Surface* current_sprite = NULL; int sprite_h = 50, sprite_w = 30; if (p->is_dead && p->death_sprite) { current_sprite = p->death_sprite; } else { int current_action_index = 0; int current_frame_index = p->nbr_frame; if (p->is_attacking) current_action_index = 2; else if (p->up) current_action_index = 3; else if (p->vitesse > 0) { if (p->vitesse > RUN_SPEED_THRESHOLD) current_action_index = (p->direction == 0) ? 4 : 5; else current_action_index = p->direction; if(current_action_index >= NB_ACTIONS || p->images[current_action_index][0]==NULL) current_action_index=p->direction;} else { current_action_index = p->direction; current_frame_index = IDLE_FRAME_INDEX; } if (current_action_index < 0 || current_action_index >= NB_ACTIONS) current_action_index = p->direction; if (current_frame_index < 0 || current_frame_index >= NB_FRAMES) current_frame_index = 0; if (current_action_index < 0 || current_action_index >= NB_ACTIONS) current_action_index = 0; if (current_action_index < NB_ACTIONS && current_frame_index < NB_FRAMES) current_sprite = p->images[current_action_index][current_frame_index]; if (!current_sprite) { if(p->images[p->direction][IDLE_FRAME_INDEX]) current_sprite = p->images[p->direction][IDLE_FRAME_INDEX]; else if (p->images[0][0]) current_sprite = p->images[0][0]; } } if (current_sprite) { sprite_w = current_sprite->w; sprite_h = current_sprite->h; } else { sprite_w=30; sprite_h=50; } box.w = sprite_w; box.h = sprite_h; box.y = GROUND_Y - box.h; return box;
}

SDL_Rect getPersoAirBoundingBox(const personne* p) { // Pour collision en l'air (précis)
    SDL_Rect box; box.x = p->position.x; box.y = p->position.y; SDL_Surface* current_sprite = NULL; int sprite_h = 50, sprite_w = 30; if (p->is_dead && p->death_sprite) { current_sprite = p->death_sprite; } else { int current_action_index = 0; int current_frame_index = p->nbr_frame; if (p->is_attacking) current_action_index = 2; else if (p->up) current_action_index = 3; else if (p->vitesse > 0) { if (p->vitesse > RUN_SPEED_THRESHOLD) current_action_index = (p->direction == 0) ? 4 : 5; else current_action_index = p->direction; if(current_action_index >= NB_ACTIONS || p->images[current_action_index][0]==NULL) current_action_index=p->direction;} else { current_action_index = p->direction; current_frame_index = IDLE_FRAME_INDEX; } if (current_action_index < 0 || current_action_index >= NB_ACTIONS) current_action_index = p->direction; if (current_frame_index < 0 || current_frame_index >= NB_FRAMES) current_frame_index = 0; if (current_action_index < 0 || current_action_index >= NB_ACTIONS) current_action_index = 0; if (current_action_index < NB_ACTIONS && current_frame_index < NB_FRAMES) current_sprite = p->images[current_action_index][current_frame_index]; if (!current_sprite) { if(p->images[p->direction][IDLE_FRAME_INDEX]) current_sprite = p->images[p->direction][IDLE_FRAME_INDEX]; else if (p->images[0][0]) current_sprite = p->images[0][0]; } } if (current_sprite) { sprite_w = current_sprite->w; sprite_h = current_sprite->h; } else { sprite_w=30; sprite_h=50; } box.w = sprite_w; box.h = sprite_h; return box;
}

void displayText(const char* message, SDL_Surface* screen, TTF_Font* font, SDL_Color color) { if (!font || !screen || !message) return; SDL_Surface* textSurface = TTF_RenderText_Blended(font, message, color); if (textSurface) { SDL_Rect textLocation; textLocation.x = (SCREEN_W - textSurface->w) / 2; textLocation.y = (SCREEN_H - textSurface->h) / 2; textLocation.w = textSurface->w; textLocation.h = textSurface->h; SDL_BlitSurface(textSurface, NULL, screen, &textLocation); SDL_FreeSurface(textSurface); } else fprintf(stderr, "TTF_RenderText Error: %s\n", TTF_GetError()); }

// Cleanup Function Modifiée pour inclure les QUATRE sons
void cleanup(personne *p, personne *p1, SDL_Surface *back, SDL_Surface *obs_img, TTF_Font *font_ui, TTF_Font *font_msg, Mix_Chunk *p1_jump_sfx, Mix_Chunk *p1_death_sfx, Mix_Chunk *p2_jump_sfx, Mix_Chunk *p2_death_sfx) {
     printf("Cleaning up resources...\n");
    if(p) freePerso(p); if(p1) freePerso(p1); if(back) SDL_FreeSurface(back); if(obs_img) SDL_FreeSurface(obs_img); if(font_ui) TTF_CloseFont(font_ui); if(font_msg) TTF_CloseFont(font_msg);
    printf("Freeing sound effects...\n");
    if(p1_jump_sfx) Mix_FreeChunk(p1_jump_sfx);
    if(p1_death_sfx) Mix_FreeChunk(p1_death_sfx);
    if(p2_jump_sfx) Mix_FreeChunk(p2_jump_sfx);
    if(p2_death_sfx) Mix_FreeChunk(p2_death_sfx);
    printf("Closing audio device...\n"); Mix_CloseAudio(); printf("Quitting SDL_mixer...\n"); Mix_Quit();
    printf("Quitting SDL subsystems...\n"); TTF_Quit(); IMG_Quit(); SDL_Quit(); printf("Exiting program.\n");
}


// --- Main Function ---
int main(int argc, char *argv[])
{
    // --- Déclarations ressources ---
    SDL_Surface *screen = NULL, *back = NULL, *obs_img = NULL; TTF_Font *font_ui = NULL, *font_msg = NULL; personne p, p1; int players_initialized = 0;

    // --- Initialisations SDL & Mixer ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError()); return 1; } if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { SDL_Quit(); return 1; } if (TTF_Init() == -1) { IMG_Quit(); SDL_Quit(); return 1; } printf("Initializing SDL_mixer...\n"); if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) { fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError()); TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1; }

    // --- Création Surface Ecran ---
    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); if (!screen) { cleanup(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL); return 1; } SDL_WM_SetCaption("SDL Personnage - All Sounds", NULL);

    // --- Chargement Assets & Sons ---
    back = IMG_Load("background.png"); obs_img = IMG_Load("obstacle.png"); const char* font_path = "arial.ttf"; int font_size_ui = 24, font_size_msg = 48; font_ui = TTF_OpenFont(font_path, font_size_ui); font_msg = TTF_OpenFont(font_path, font_size_msg);
    printf("Loading sound effect 'jump.wav' (P1 Jump/Collision)...\n"); p1_jump_sound_collision = Mix_LoadWAV("jump.wav"); if (!p1_jump_sound_collision) { fprintf(stderr, "Mix_LoadWAV Error for 'jump.wav': %s\n", Mix_GetError()); cleanup(NULL, NULL, back, obs_img, font_ui, font_msg, NULL, NULL, NULL, NULL); return 1; }
    printf("Loading sound effect 'death.wav' (P1 Death)...\n"); p1_death_sound = Mix_LoadWAV("death.wav"); if (!p1_death_sound) { fprintf(stderr, "Mix_LoadWAV Error for 'death.wav': %s\n", Mix_GetError()); cleanup(NULL, NULL, back, obs_img, font_ui, font_msg, p1_jump_sound_collision, NULL, NULL, NULL); return 1; }
    printf("Loading sound effect 'man-saut.wav' (P2 Jump)...\n"); p2_jump_sound = Mix_LoadWAV("man-saut.wav"); if (!p2_jump_sound) { fprintf(stderr, "Mix_LoadWAV Error for 'death man.wav' (P2 Jump): %s\n", Mix_GetError()); cleanup(NULL, NULL, back, obs_img, font_ui, font_msg, p1_jump_sound_collision, p1_death_sound, NULL, NULL); return 1; }
    printf("Loading sound effect 'death man.wav' (P2 Death)...\n"); p2_death_sound = Mix_LoadWAV("death man.wav"); if (!p2_death_sound) { fprintf(stderr, "Mix_LoadWAV Error for 'death man.wav' (P2 Death): %s\n", Mix_GetError()); cleanup(NULL, NULL, back, obs_img, font_ui, font_msg, p1_jump_sound_collision, p1_death_sound, p2_jump_sound, NULL); return 1; }
    if (!back || !obs_img || !font_ui || !font_msg) { cleanup(NULL, NULL, back, obs_img, font_ui, font_msg, p1_jump_sound_collision, p1_death_sound, p2_jump_sound, p2_death_sound); return 1; }

    // --- Initialisation Joueurs ---
    initialiserperso(&p); initialiserperso2(&p1); players_initialized = 1;

    // --- Variables Boucle Jeu ---
    SDL_Event event; int running = 1; Uint32 t_prev = SDL_GetTicks(); const float base_speed = 200.0; int p1_move_left = 0, p1_move_right = 0, p2_move_left = 0, p2_move_right = 0; int game_over = 0; SDL_Color message_color = {255, 200, 0, 0};
    int p1_death_sound_played = 0;
    int p2_death_sound_played = 0; // Flag pour son mort P2

    printf("Starting main game loop...\n");
    // --- Boucle Principale ---
    while (running) {
        Uint32 now = SDL_GetTicks(); Uint32 dt = (now > t_prev) ? (now - t_prev) : 1; if(dt == 0) dt = 1; t_prev = now;

        // --- Gestion Événements ---
        if (!game_over) { while (SDL_PollEvent(&event)) { if (event.type == SDL_QUIT) running = 0; else if (event.type == SDL_KEYDOWN) { switch(event.key.keysym.sym){ /* P1 */ case SDLK_RIGHT: if (!p.is_dead) p1_move_right = 1; break; case SDLK_LEFT:  if (!p.is_dead) p1_move_left = 1;  break; case SDLK_UP: if (!p.is_dead && !p.is_attacking && p.jumps_remaining > 0) { p.vitesse_saut = INITIAL_JUMP_VELOCITY; p.jumps_remaining--; p.up = 1; p.nbr_frame = 0; if (p1_jump_sound_collision) Mix_PlayChannel(-1, p1_jump_sound_collision, 0); } break; case SDLK_SPACE: if (!p.is_dead) startAttack(&p, now); break; /* P2 */ case SDLK_d: if (!p1.is_dead) p2_move_right = 1; break; case SDLK_q: if (!p1.is_dead) p2_move_left = 1;  break; case SDLK_z: if (!p1.is_dead && !p1.is_attacking && p1.jumps_remaining > 0) { p1.vitesse_saut = INITIAL_JUMP_VELOCITY; p1.jumps_remaining--; p1.up = 1; p1.nbr_frame = 0; if (p2_jump_sound) Mix_PlayChannel(-1, p2_jump_sound, 0); } break; case SDLK_e: if (!p1.is_dead) startAttack(&p1, now); break; /* General */ case SDLK_ESCAPE: running=0; break; }} else if (event.type == SDL_KEYUP) { switch(event.key.keysym.sym){ case SDLK_RIGHT: p1_move_right = 0; break; case SDLK_LEFT:  p1_move_left = 0; break; case SDLK_d: p2_move_right = 0; break; case SDLK_q: p2_move_left = 0; break; }} }} else { while (SDL_PollEvent(&event)) { if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) running = 0; } }

        // --- MàJ Vitesse Mouvement ---
        if (!p.is_dead && !game_over) { if (p1_move_right && !p1_move_left) { p.vitesse = base_speed; p.direction = 0; } else if (p1_move_left && !p1_move_right) { p.vitesse = base_speed; p.direction = 1; } else { p.vitesse = 0; } } else { p.vitesse = 0; } if (!p1.is_dead && !game_over) { if (p2_move_right && !p2_move_left) { p1.vitesse = base_speed; p1.direction = 0; } else if (p2_move_left && !p2_move_right) { p1.vitesse = base_speed; p1.direction = 1; } else { p1.vitesse = 0; } } else { p1.vitesse = 0; }

        // --- MàJ Logique Jeu ---
        moveperso(&p, dt); saut(&p, GROUND_Y, dt); updateAnimation(&p, now); moveperso(&p1, dt); saut(&p1, GROUND_Y, dt); updateAnimation(&p1, now);

        // --- Collisions ---
        if (!game_over) { SDL_Rect p_box_air = getPersoAirBoundingBox(&p); SDL_Rect p1_box_air = getPersoAirBoundingBox(&p1);
            /* Collision P1 */ if (!p.is_dead && intersects(p_box_air, obstacle)) { int was_alive_before = !p.is_dead; if (p1_jump_sound_collision) Mix_PlayChannel(-1, p1_jump_sound_collision, 0); perdre_vie(&p); if (was_alive_before && p.is_dead && !p1_death_sound_played) { if (p1_death_sound) Mix_PlayChannel(-1, p1_death_sound, 0); p1_death_sound_played = 1; } if (!p.is_dead) { /* push back */ } }
            /* Collision P2 */ if (!p1.is_dead && intersects(p1_box_air, obstacle)) { int was_p2_alive_before = !p1.is_dead; perdre_vie(&p1); if (was_p2_alive_before && p1.is_dead && !p2_death_sound_played) { if (p2_death_sound) Mix_PlayChannel(-1, p2_death_sound, 0); p2_death_sound_played = 1; } if (!p1.is_dead) { /* push back */ } } }

        // --- Check Game Over ---
        if (!game_over && p.is_dead && p1.is_dead) { game_over = 1; printf("GAME OVER!\n");}

        // --- Rendu ---
        SDL_BlitSurface(back, NULL, screen, NULL); SDL_BlitSurface(obs_img, NULL, screen, &obstacle); afficherperso(p, screen); afficherperso(p1, screen); affichervie(p, screen, 10, 10); afficherScore(p, screen, font_ui, 150, 10); affichervie(p1, screen, 10, 70); afficherScore(p1, screen, font_ui, 150, 70); if (game_over) displayText("Game Over", screen, font_msg, message_color); else if (p.is_dead && !p1.is_dead) displayText("Joueur 1 a perdu!", screen, font_msg, message_color); else if (!p.is_dead && p1.is_dead) displayText("Joueur 2 a perdu!", screen, font_msg, message_color); SDL_Flip(screen);

        // --- Limitation FPS ---
        Uint32 frame_time = SDL_GetTicks() - now; if (frame_time < 16) SDL_Delay(16 - frame_time);

    } // --- Fin Boucle Jeu ---

    // --- Nettoyage ---
    cleanup(players_initialized ? &p : NULL, players_initialized ? &p1 : NULL, back, obs_img, font_ui, font_msg, p1_jump_sound_collision, p1_death_sound, p2_jump_sound, p2_death_sound);

    return 0;
}
