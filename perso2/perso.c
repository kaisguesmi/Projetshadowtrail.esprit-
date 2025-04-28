// perso.c
#include "perso.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h> // Include TTF here too for implementation
#include <stdio.h>

// charge une série d'images numérotées 0..NB_FRAMES-1 depuis folder
static void charger_animation(SDL_Surface* images[], const char* folder)
{
    char filename[128];
    printf("Loading animation from: %s\n", folder); // Debug print
    for (int i = 0; i < NB_FRAMES; i++) {
        snprintf(filename, sizeof(filename), "%s/%d.png", folder, i);
        images[i] = IMG_Load(filename);
        if (!images[i]) {
            fprintf(stderr, "Erreur chargement %s: %s\n", filename, IMG_GetError());
        } else {
             printf(" > Loaded %s\n", filename);
        }
    }
}

// charge les icônes de vie 0..LIFE_STATES-1
static void charger_life_images(SDL_Surface* life_images[])
{
    char filename[128];
    printf("Loading life images from: life/\n");
    for (int i = 0; i < LIFE_STATES; i++) {
        snprintf(filename, sizeof(filename), "life/%d.png", i);
        life_images[i] = IMG_Load(filename);
        if (!life_images[i]) {
            fprintf(stderr, "Erreur chargement %s: %s\n", filename, IMG_GetError());
        } else {
             printf(" > Loaded %s\n", filename);
        }
    }
}

void initialiserperso(personne *p)
{
    p->direction = 0;
    p->nbr_frame = 0;
    Uint32 now = SDL_GetTicks();
    p->last_frame_time = now;
    p->last_attack_time = 0;
    p->attack_cooldown = 500;
    p->walk_interval = WALK_FRAME_INTERVAL;
    p->run_interval = RUN_FRAME_INTERVAL;
    p->jump_interval = JUMP_FRAME_INTERVAL;
    p->attack_interval = ATTACK_FRAME_INTERVAL;
    p->vitesse = 0;
    p->vitesse_saut = 0;
    p->up = 0;
    p->is_attacking = 0;
    p->life_state = 0; // Commence avec la vie pleine (index 0)
    p->position.x = 10;
    p->score = 0;
    p->last_hit_time = 0;
    p->is_dead = 0; // Initialiser comme vivant
    p->death_sprite = NULL; // Initialiser le pointeur

    // Init image pointers
    for(int i=0; i<NB_ACTIONS; ++i) for(int j=0; j<NB_FRAMES; ++j) p->images[i][j] = NULL;
    for(int i=0; i<LIFE_STATES; ++i) p->life_images[i] = NULL;

    // Load images
    charger_animation(p->images[0], "animation/walk_right");
    charger_animation(p->images[1], "animation/walk_left");
    charger_animation(p->images[2], "animation/attack");
    charger_animation(p->images[3], "animation/jump");
    charger_animation(p->images[4], "animation/run");
    charger_life_images(p->life_images);
    p->death_sprite = IMG_Load("die/0.png"); // Charger l'image de mort
    if (!p->death_sprite) {
        fprintf(stderr, "Erreur chargement die/0.png: %s\n", IMG_GetError());
    } else {
         printf(" > Loaded die/0.png\n");
    }

    // Set position Y
    int sprite_h = 0;
    if (p->images[0][0]) sprite_h = p->images[0][0]->h; else sprite_h = 50;
    p->position.y = GROUND_Y - sprite_h;
    p->position.w = 0; p->position.h = 0;
}

void initialiserperso2(personne *p1)
{
    p1->direction = 0;
    p1->nbr_frame = 0;
    Uint32 now = SDL_GetTicks();
    p1->last_frame_time = now;
    p1->last_attack_time = 0;
    p1->attack_cooldown = 500;
    p1->walk_interval = WALK_FRAME_INTERVAL;
    p1->run_interval = RUN_FRAME_INTERVAL;
    p1->jump_interval = JUMP_FRAME_INTERVAL;
    p1->attack_interval = ATTACK_FRAME_INTERVAL;
    p1->vitesse = 0;
    p1->vitesse_saut = 0;
    p1->up = 0;
    p1->is_attacking = 0;
    p1->life_state = 0;
    p1->position.x = 300;
    p1->score = 0;
    p1->last_hit_time = 0;
    p1->is_dead = 0; // Initialiser comme vivant
    p1->death_sprite = NULL; // Initialiser le pointeur

    // Init image pointers
    for(int i=0; i<NB_ACTIONS; ++i) for(int j=0; j<NB_FRAMES; ++j) p1->images[i][j] = NULL;
    for(int i=0; i<LIFE_STATES; ++i) p1->life_images[i] = NULL;

    // Load images
    charger_animation(p1->images[0], "new_perso_p1/walk_right");
    charger_animation(p1->images[1], "new_perso_p1/walk_left");
    charger_animation(p1->images[2], "new_perso_p1/attack");
    charger_animation(p1->images[3], "new_perso_p1/jump");
    charger_animation(p1->images[4], "new_perso_p1/run");
    charger_life_images(p1->life_images);
    // Charger l'image de mort (on suppose la même)
    p1->death_sprite = IMG_Load("die/0.png");
    if (!p1->death_sprite) {
        fprintf(stderr, "Erreur chargement die/0.png pour p1: %s\n", IMG_GetError());
    } else {
        printf(" > Loaded die/0.png for p1\n");
    }

    // Set position Y
    int sprite_h = 0;
    if (p1->images[0][0]) sprite_h = p1->images[0][0]->h; else sprite_h = 50;
    p1->position.y = GROUND_Y - sprite_h;
    p1->position.w = 0; p1->position.h = 0;
}


void freePerso(personne *p) {
    printf("Freeing perso surfaces...\n");
    for (int i = 0; i < NB_ACTIONS; ++i) {
        for (int j = 0; j < NB_FRAMES; ++j) {
            if (p->images[i][j]) { SDL_FreeSurface(p->images[i][j]); p->images[i][j] = NULL; }
        }
    }
    for (int i = 0; i < LIFE_STATES; ++i) {
        if (p->life_images[i]) { SDL_FreeSurface(p->life_images[i]); p->life_images[i] = NULL; }
    }
    if (p->death_sprite) { // Libérer l'image de mort
        SDL_FreeSurface(p->death_sprite);
        p->death_sprite = NULL;
    }
}

void afficherperso(const personne p, SDL_Surface *screen)
{
    // Si le personnage est mort, afficher l'image de mort et sortir
    if (p.is_dead) {
        if (p.death_sprite) {
            SDL_Rect dest_pos = p.position;
            // Ajuster Y si l'image de mort est de taille différente
            // int sprite_h_death = p.death_sprite->h;
            // dest_pos.y = GROUND_Y - sprite_h_death; // Recalculer si besoin
            SDL_BlitSurface(p.death_sprite, NULL, screen, &dest_pos);
        }
        return; // Ne pas afficher l'animation normale
    }

    // --- Si vivant, continuer avec l'affichage normal ---
    int current_action_index = 0;
    int current_frame_index = p.nbr_frame;
    Uint32 now = SDL_GetTicks();
    int is_invincible = (now - p.last_hit_time < INVINCIBILITY_DURATION);

    // Déterminer l'action actuelle
    if (p.is_attacking) { current_action_index = 2; }
    else if (p.up) { current_action_index = 3; }
    else if (p.vitesse > 0) {
        if (p.vitesse > RUN_SPEED_THRESHOLD) {
             current_action_index = (p.direction == 0) ? 4 : 5;
             if (current_action_index == 5 && p.images[5][0] == NULL) { current_action_index = 1; }
        } else { current_action_index = p.direction; }
    } else {
        current_action_index = p.direction;
        current_frame_index = IDLE_FRAME_INDEX;
    }

    // Vérifier les indices
    if (current_action_index < 0 || current_action_index >= NB_ACTIONS ||
        current_frame_index < 0 || current_frame_index >= NB_FRAMES) {
        current_action_index = 0; current_frame_index = 0;
    }

    SDL_Surface* sprite_to_draw = p.images[current_action_index][current_frame_index];
    SDL_Rect dest_pos = p.position;

    // Gérer le clignotement si invincible
    int should_draw = 1;
    if (is_invincible && ((now / 100) % 2 != 0)) {
        should_draw = 0;
    }

    // Dessiner si nécessaire
    if (should_draw) {
        if (sprite_to_draw != NULL) {
            SDL_BlitSurface(sprite_to_draw, NULL, screen, &dest_pos);
        } else {
             sprite_to_draw = p.images[p.direction][IDLE_FRAME_INDEX];
             if (sprite_to_draw != NULL) { SDL_BlitSurface(sprite_to_draw, NULL, screen, &dest_pos); }
        }
    }
}


void affichervie(const personne p, SDL_Surface *screen, int x, int y)
{
    // Ne pas afficher la barre de vie si mort, ou afficher le dernier état
     int state_to_show = p.life_state;
     // if (p.is_dead) return; // Option 1: Cacher la vie si mort
     // Option 2: Montrer la vie vide (dernier état) si mort
     if (p.is_dead && state_to_show < LIFE_STATES -1) {
          state_to_show = LIFE_STATES -1;
     }


    if (state_to_show >= 0 && state_to_show < LIFE_STATES && p.life_images[state_to_show] != NULL) {
        SDL_Rect dest = { (Sint16)x, (Sint16)y, 0, 0 };
        SDL_BlitSurface(p.life_images[state_to_show], NULL, screen, &dest);
    }
}

void afficherScore(const personne p, SDL_Surface *screen, TTF_Font *font, int x, int y)
{
   if (!font) return;
   // Utiliser une couleur différente si mort? Non, score final visible.
   SDL_Color textColor = { 255, 255, 255, 0 };
   char score_text[64];
   snprintf(score_text, sizeof(score_text), "Score: %d", p.score);
   SDL_Surface* textSurface = TTF_RenderText_Solid(font, score_text, textColor);
   if (textSurface) {
       SDL_Rect textLocation = { (Sint16)x, (Sint16)y, 0, 0 };
       SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
       SDL_FreeSurface(textSurface);
   } else { fprintf(stderr, "TTF_RenderText_Solid Error: %s\n", TTF_GetError()); }
}


void moveperso(personne *p, Uint32 dt)
{
    if (p->is_dead) return; // Ne pas bouger si mort
    if (p->is_attacking || p->vitesse == 0) return;

    p->score += 5; // Score pour mouvement horizontal

    double dt_seconds = dt / 1000.0;
    double dx = p->vitesse * dt_seconds;
    p->position.x += (p->direction == 0 ? dx : -dx);

    // Boundary checks
    int sprite_w = 0; if (p->images[0][0]) sprite_w = p->images[0][0]->w; else sprite_w = 30;
    if (p->position.x < 0) { p->position.x = 0; p->vitesse = 0; }
    int max_x = SCREEN_W - sprite_w; if (max_x < 0) max_x = 0;
    if (p->position.x > max_x) { p->position.x = max_x; p->vitesse = 0; }
}

void updateAnimation(personne* p, Uint32 now)
{
    if (p->is_dead) return; // Pas d'animation si mort

    Uint32 current_interval = p->walk_interval;
    int current_action_frame_count = NB_FRAMES;
    int current_action = p->direction;
    int loop_animation = 1;
    if (p->is_attacking) { current_interval=p->attack_interval; current_action=2; loop_animation=0; }
    else if (p->up) { current_interval=p->jump_interval; current_action=3; /*loop_animation=0;*/ }
    else if (p->vitesse > 0) {
        if (p->vitesse > RUN_SPEED_THRESHOLD) {
            current_interval=p->run_interval; current_action=(p->direction==0)?4:5;
            if (current_action==5 && p->images[5][0]==NULL){ current_action=1; current_interval=p->walk_interval; }
        } else { current_interval=p->walk_interval; current_action=p->direction; }
    } else { p->nbr_frame=IDLE_FRAME_INDEX; p->last_frame_time=now; return; }
    if (now - p->last_frame_time >= current_interval) {
        p->last_frame_time = now; p->nbr_frame++;
        if (p->nbr_frame >= current_action_frame_count) {
            if (loop_animation) { p->nbr_frame = 0; }
            else { p->nbr_frame = current_action_frame_count - 1; if (p->is_attacking) p->is_attacking = 0; }
        }
    }
}

void saut(personne* p, int ground_y, Uint32 dt)
{
    if (p->is_dead) return; // Pas de saut si mort

    const double GRAVITY = 980.0 * 2.0; const double INITIAL_JUMP_VELOCITY = -550.0;
    int sprite_h = 0; if (p->images[0][0]) sprite_h = p->images[0][0]->h; else sprite_h = 50;
    int current_ground_y = ground_y - sprite_h;
    if (p->up == 1 && p->vitesse_saut == 0 && p->position.y >= current_ground_y) {
        p->vitesse_saut = INITIAL_JUMP_VELOCITY; p->position.y = current_ground_y - 1; p->nbr_frame = 0;
    }
    if (p->up || p->position.y < current_ground_y) {
        p->score += 5; // Score pour être en l'air
        double dt_seconds = dt / 1000.0; p->vitesse_saut += GRAVITY * dt_seconds; p->position.y += p->vitesse_saut * dt_seconds;
        if (p->position.y >= current_ground_y) { p->position.y = current_ground_y; p->vitesse_saut = 0; p->up = 0; p->nbr_frame = 0; }
        else { p->up = 1; }
    } else { p->vitesse_saut = 0; }
}


void startAttack(personne* p, Uint32 now) {
    if (p->is_dead) return; // Pas d'attaque si mort

    if (!p->is_attacking && !p->up && (now - p->last_attack_time >= p->attack_cooldown)) {
        printf("Player attacking!\n");
        p->is_attacking = 1; p->nbr_frame = 0; p->last_frame_time = now; p->last_attack_time = now;
        p->score += 5;
    }
}

void perdre_vie(personne* p)
{
    if (p->is_dead) return; // Déjà mort

    Uint32 now = SDL_GetTicks();
    if (now - p->last_hit_time < INVINCIBILITY_DURATION) {
        return; // Invincible
    }

    // Prendre des dégâts
    if (p->life_state < LIFE_STATES - 1) { // Si pas encore au dernier état
        p->life_state++;
        printf("Player lost life, state is now: %d\n", p->life_state);
        p->last_hit_time = now; // Activer l'invincibilité

        // Vérifier si c'est le dernier état (la mort)
        if (p->life_state == LIFE_STATES - 1) {
            printf("Player is DEAD!\n");
            p->is_dead = 1; // Marquer comme mort
            // Arrêter toute activité
            p->vitesse = 0;
            p->vitesse_saut = 0;
            p->up = 0;
            p->is_attacking = 0;
        }
    } else {
        // Déjà au dernier état (devrait être mort), on met juste à jour le timer d'invincibilité
        // pour éviter les messages en boucle si on reste sur l'obstacle.
        p->last_hit_time = now;
        // S'assurer que le flag is_dead est bien mis (sécurité)
        if (!p->is_dead) {
             printf("Player life already at minimum state, marking as dead.\n");
             p->is_dead = 1;
             p->vitesse = 0; p->vitesse_saut = 0; p->up = 0; p->is_attacking = 0;
        }
    }
}
