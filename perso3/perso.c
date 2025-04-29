// perso.c
#include "perso.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>

// charge une série d'images numérotées 0..NB_FRAMES-1 depuis folder
static void charger_animation(SDL_Surface* images[], const char* folder)
{
    char filename[128]; printf("Loading animation from: %s\n", folder);
    for (int i = 0; i < NB_FRAMES; i++) { snprintf(filename, sizeof(filename), "%s/%d.png", folder, i); images[i] = IMG_Load(filename); if (!images[i]) fprintf(stderr, "Erreur chargement %s: %s\n", filename, IMG_GetError()); else printf(" > Loaded %s\n", filename); }
}
// charge les icônes de vie 0..LIFE_STATES-1
static void charger_life_images(SDL_Surface* life_images[])
{
    char filename[128]; printf("Loading life images from: life/\n");
    for (int i = 0; i < LIFE_STATES; i++) { snprintf(filename, sizeof(filename), "life/%d.png", i); life_images[i] = IMG_Load(filename); if (!life_images[i]) fprintf(stderr, "Erreur chargement %s: %s\n", filename, IMG_GetError()); else printf(" > Loaded %s\n", filename); }
}

void initialiserperso(personne *p)
{
    p->direction = 0; p->nbr_frame = 0; Uint32 now = SDL_GetTicks(); p->last_frame_time = now; p->last_attack_time = 0; p->attack_cooldown = 500; p->walk_interval = WALK_FRAME_INTERVAL; p->run_interval = RUN_FRAME_INTERVAL; p->jump_interval = JUMP_FRAME_INTERVAL; p->attack_interval = ATTACK_FRAME_INTERVAL; p->vitesse = 0; p->vitesse_saut = 0; p->up = 0; p->jumps_remaining = MAX_JUMPS; p->is_attacking = 0; p->life_state = 0; p->position.x = 10; p->score = 0; p->last_hit_time = 0; p->is_dead = 0; p->death_sprite = NULL;
    for(int i=0; i<NB_ACTIONS; ++i) for(int j=0; j<NB_FRAMES; ++j) p->images[i][j] = NULL; for(int i=0; i<LIFE_STATES; ++i) p->life_images[i] = NULL;
    charger_animation(p->images[0], "animation/walk_right"); charger_animation(p->images[1], "animation/walk_left"); charger_animation(p->images[2], "animation/attack"); charger_animation(p->images[3], "animation/jump"); charger_animation(p->images[4], "animation/run"); charger_life_images(p->life_images); p->death_sprite = IMG_Load("die/0.png"); if (!p->death_sprite) fprintf(stderr, "Erreur chargement die/0.png: %s\n", IMG_GetError()); else printf(" > Loaded die/0.png\n");
    int sprite_h = 50; if (p->images[0][IDLE_FRAME_INDEX]) sprite_h = p->images[0][IDLE_FRAME_INDEX]->h; else if (p->images[0][0]) sprite_h = p->images[0][0]->h; p->position.y = GROUND_Y - sprite_h; p->position.w = 0; p->position.h = 0;
}

void initialiserperso2(personne *p1)
{
    p1->direction = 0; p1->nbr_frame = 0; Uint32 now = SDL_GetTicks(); p1->last_frame_time = now; p1->last_attack_time = 0; p1->attack_cooldown = 500; p1->walk_interval = WALK_FRAME_INTERVAL; p1->run_interval = RUN_FRAME_INTERVAL; p1->jump_interval = JUMP_FRAME_INTERVAL; p1->attack_interval = ATTACK_FRAME_INTERVAL; p1->vitesse = 0; p1->vitesse_saut = 0; p1->up = 0; p1->jumps_remaining = MAX_JUMPS; p1->is_attacking = 0; p1->life_state = 0; p1->position.x = 300; p1->score = 0; p1->last_hit_time = 0; p1->is_dead = 0; p1->death_sprite = NULL;
    for(int i=0; i<NB_ACTIONS; ++i) for(int j=0; j<NB_FRAMES; ++j) p1->images[i][j] = NULL; for(int i=0; i<LIFE_STATES; ++i) p1->life_images[i] = NULL;
    charger_animation(p1->images[0], "new_perso_p1/walk_right"); charger_animation(p1->images[1], "new_perso_p1/walk_left"); charger_animation(p1->images[2], "new_perso_p1/attack"); charger_animation(p1->images[3], "new_perso_p1/jump"); charger_animation(p1->images[4], "new_perso_p1/run"); charger_life_images(p1->life_images); p1->death_sprite = IMG_Load("die/0.png"); if (!p1->death_sprite) fprintf(stderr, "Erreur chargement die/0.png pour p1: %s\n", IMG_GetError()); else printf(" > Loaded die/0.png for p1\n");
    int sprite_h = 50; if (p1->images[0][IDLE_FRAME_INDEX]) sprite_h = p1->images[0][IDLE_FRAME_INDEX]->h; else if (p1->images[0][0]) sprite_h = p1->images[0][0]->h; p1->position.y = GROUND_Y - sprite_h; p1->position.w = 0; p1->position.h = 0;
}

void freePerso(personne *p) {
    printf("Freeing perso surfaces...\n"); for (int i = 0; i < NB_ACTIONS; ++i) for (int j = 0; j < NB_FRAMES; ++j) if (p->images[i][j]) { SDL_FreeSurface(p->images[i][j]); p->images[i][j] = NULL; } for (int i = 0; i < LIFE_STATES; ++i) if (p->life_images[i]) { SDL_FreeSurface(p->life_images[i]); p->life_images[i] = NULL; } if (p->death_sprite) { SDL_FreeSurface(p->death_sprite); p->death_sprite = NULL; }
}

void afficherperso(const personne p, SDL_Surface *screen) {
    SDL_Rect dest_pos; if (p.is_dead) { if (p.death_sprite) { dest_pos.x = p.position.x; dest_pos.y = GROUND_Y - p.death_sprite->h; SDL_BlitSurface(p.death_sprite, NULL, screen, &dest_pos); } return; } int current_action_index = 0; int current_frame_index = p.nbr_frame; Uint32 now = SDL_GetTicks(); int is_invincible = (now - p.last_hit_time < INVINCIBILITY_DURATION); if (p.is_attacking) current_action_index = 2; else if (p.up) current_action_index = 3; else if (p.vitesse > 0) { if (p.vitesse > RUN_SPEED_THRESHOLD) { current_action_index = (p.direction == 0) ? 4 : 5; if (current_action_index >= NB_ACTIONS || p.images[current_action_index][0] == NULL) current_action_index = p.direction; } else current_action_index = p.direction; } else { current_action_index = p.direction; current_frame_index = IDLE_FRAME_INDEX; } if (current_action_index < 0 || current_action_index >= NB_ACTIONS) current_action_index = 0; if (current_frame_index < 0 || current_frame_index >= NB_FRAMES) current_frame_index = 0; if (current_action_index < 0 || current_action_index >= NB_ACTIONS || current_frame_index < 0 || current_frame_index >= NB_FRAMES || p.images[current_action_index][current_frame_index] == NULL) { current_action_index = p.direction; current_frame_index = 0; if (current_action_index < 0 || current_action_index >= NB_ACTIONS || p.images[current_action_index][current_frame_index] == NULL) return; } SDL_Surface* sprite_to_draw = p.images[current_action_index][current_frame_index]; dest_pos = p.position; int should_draw = 1; if (is_invincible && ((now / 100) % 2 != 0)) should_draw = 0; if (should_draw) SDL_BlitSurface(sprite_to_draw, NULL, screen, &dest_pos);
}

void affichervie(const personne p, SDL_Surface *screen, int x, int y) {
    int state_to_show = p.life_state; if (p.is_dead && state_to_show < LIFE_STATES - 1) state_to_show = LIFE_STATES - 1; if (state_to_show >= 0 && state_to_show < LIFE_STATES && p.life_images[state_to_show] != NULL) { SDL_Rect dest = { (Sint16)x, (Sint16)y, 0, 0 }; SDL_BlitSurface(p.life_images[state_to_show], NULL, screen, &dest); }
}

void afficherScore(const personne p, SDL_Surface *screen, TTF_Font *font, int x, int y) {
   if (!font) return; SDL_Color textColor = { 255, 255, 255, 0 }; char score_text[64]; snprintf(score_text, sizeof(score_text), "Score: %d", p.score); SDL_Surface* textSurface = TTF_RenderText_Solid(font, score_text, textColor); if (textSurface) { SDL_Rect textLocation = { (Sint16)x, (Sint16)y, 0, 0 }; SDL_BlitSurface(textSurface, NULL, screen, &textLocation); SDL_FreeSurface(textSurface); } else fprintf(stderr, "TTF_RenderText_Solid Error: %s\n", TTF_GetError());
}

void moveperso(personne *p, Uint32 dt) {
    if (p->is_dead || p->vitesse == 0) return; double dt_seconds = dt / 1000.0; double dx = p->vitesse * dt_seconds; p->position.x += (p->direction == 0 ? dx : -dx); int sprite_w = 30; if (p->images[p->direction][IDLE_FRAME_INDEX]) sprite_w = p->images[p->direction][IDLE_FRAME_INDEX]->w; else if (p->images[0][0]) sprite_w = p->images[0][0]->w; if (p->position.x < 0) p->position.x = 0; int max_x = SCREEN_W - sprite_w; if (max_x < 0) max_x = 0; if (p->position.x > max_x) p->position.x = max_x;
}

void updateAnimation(personne* p, Uint32 now) {
    if (p->is_dead) return; Uint32 current_interval = p->walk_interval; int current_action_frame_count = NB_FRAMES; int current_action_for_anim = p->direction; int loop_animation = 1; if (p->is_attacking) { current_interval = p->attack_interval; current_action_for_anim = 2; loop_animation = 0; } else if (p->up) { current_interval = p->jump_interval; current_action_for_anim = 3; } else if (p->vitesse > 0) { if (p->vitesse > RUN_SPEED_THRESHOLD) { current_action_for_anim = (p->direction == 0) ? 4 : 5; if (current_action_for_anim >= NB_ACTIONS || p->images[current_action_for_anim][0] == NULL) { current_action_for_anim = p->direction; current_interval = p->walk_interval; } else { current_interval = p->run_interval; } } else { current_action_for_anim = p->direction; current_interval = p->walk_interval; } } else { p->nbr_frame = IDLE_FRAME_INDEX; p->last_frame_time = now; return; } if (current_action_for_anim < 0 || current_action_for_anim >= NB_ACTIONS) return; if (now - p->last_frame_time >= current_interval) { p->last_frame_time = now; p->nbr_frame++; int actual_frame_count = 0; for (int k = 0; k < NB_FRAMES; ++k) { if (current_action_for_anim < NB_ACTIONS && k < NB_FRAMES && p->images[current_action_for_anim][k] != NULL) actual_frame_count++; else break; } if (actual_frame_count == 0) actual_frame_count = 1; if (p->nbr_frame >= actual_frame_count) { if (loop_animation) p->nbr_frame = 0; else { p->nbr_frame = actual_frame_count - 1; if (p->is_attacking) p->is_attacking = 0; } } }
}

void saut(personne* p, int ground_y, Uint32 dt) {
    if (p->is_dead) return; if (p->up) { double dt_seconds = dt / 1000.0; p->vitesse_saut += GRAVITY * dt_seconds; p->position.y += p->vitesse_saut * dt_seconds; int sprite_h = 50; SDL_Surface* current_sprite = NULL; int action_idx = 3; int frame_idx = p->nbr_frame; if(action_idx < NB_ACTIONS && frame_idx >= 0 && frame_idx < NB_FRAMES && p->images[action_idx][frame_idx] != NULL){ current_sprite = p->images[action_idx][frame_idx]; } else { if(p->images[p->direction][IDLE_FRAME_INDEX]) current_sprite = p->images[p->direction][IDLE_FRAME_INDEX]; else if(p->images[0][0]) current_sprite = p->images[0][0]; } if(current_sprite) sprite_h = current_sprite->h; else sprite_h = 50; int target_y_on_ground = ground_y - sprite_h; if (p->position.y >= target_y_on_ground && p->vitesse_saut >= 0) { p->position.y = target_y_on_ground; p->vitesse_saut = 0; p->up = 0; p->jumps_remaining = MAX_JUMPS; p->nbr_frame = IDLE_FRAME_INDEX; } }
}

void startAttack(personne* p, Uint32 now) {
    if (p->is_dead) return; if (!p->is_attacking && (now - p->last_attack_time >= p->attack_cooldown)) { p->is_attacking = 1; p->nbr_frame = 0; p->last_frame_time = now; p->last_attack_time = now; p->score += 5; }
}

void perdre_vie(personne* p) {
    if (p->is_dead) return; Uint32 now = SDL_GetTicks(); if (now - p->last_hit_time < INVINCIBILITY_DURATION) return; if (p->life_state < LIFE_STATES - 1) { p->life_state++; printf("Player lost life, state is now: %d / %d\n", p->life_state, LIFE_STATES - 1 ); p->last_hit_time = now; if (p->life_state == LIFE_STATES - 1) { printf("Player is DEAD! (Sound will be triggered in main loop)\n"); p->is_dead = 1; p->vitesse = 0; p->vitesse_saut = 0; p->up = 0; p->is_attacking = 0; } } else { if (!p->is_dead) { printf("Player life already minimum, marking dead.\n"); p->is_dead = 1; p->vitesse = 0; p->vitesse_saut = 0; p->up = 0; p->is_attacking = 0; } p->last_hit_time = now; }
}
