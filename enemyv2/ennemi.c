/*
ennemi.c - Implémentation des fonctions du module ennemi
*/

#include "ennemi.h"
#include <stdio.h>
#include <string.h>
#include <math.h> // Make sure it's included

// Define _MAX_PATH if not available (e.g., on Linux/macOS)
#ifndef _MAX_PATH
#define _MAX_PATH 260 // Or appropriate value for your system
#endif

// --- HELPER FUNCTIONS for SDL 1.2 manual surface flipping ---
// Gets the pixel value at (x, y) from a surface
Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) { return 0; }
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1: return *p;
        case 2: return *(Uint16 *)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) return p[0] << 16 | p[1] << 8 | p[2];
            else return p[0] | p[1] << 8 | p[2] << 16;
        case 4: return *(Uint32 *)p;
        default: return 0;
    }
}
// Puts a pixel value at (x, y) on a surface
void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
     if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) { return; }
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1: *p = pixel; break;
        case 2: *(Uint16 *)p = pixel; break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) { p[0] = (pixel >> 16) & 0xff; p[1] = (pixel >> 8) & 0xff; p[2] = pixel & 0xff; }
            else { p[0] = pixel & 0xff; p[1] = (pixel >> 8) & 0xff; p[2] = (pixel >> 16) & 0xff; } break;
        case 4: *(Uint32 *)p = pixel; break;
    }
}
// --- END HELPER FUNCTIONS ---


/* Initialise une animation */
Animation* initialiser_animation(const char *prefixe_image, int nb_frames, int delai) {
    Animation *anim = (Animation*) malloc(sizeof(Animation));
    if (!anim) { fprintf(stderr, "Erreur alloc anim\n"); return NULL; }
    anim->frames = (SDL_Surface**) malloc(sizeof(SDL_Surface*) * nb_frames);
    if (!anim->frames) { fprintf(stderr, "Erreur alloc frames\n"); free(anim); return NULL; }
    for (int i = 0; i < nb_frames; i++) { anim->frames[i] = NULL; }

    char chemin_image[_MAX_PATH];
    int success = 1;
    for (int i = 0; i < nb_frames; i++) {
        snprintf(chemin_image, _MAX_PATH, "%s%d.png", prefixe_image, i);
        anim->frames[i] = IMG_Load(chemin_image);
        if (!anim->frames[i]) { fprintf(stderr, "Cannot load %s: %s\n", chemin_image, IMG_GetError()); success = 0; break; }
        if (SDL_SetColorKey(anim->frames[i], SDL_SRCCOLORKEY, SDL_MapRGB(anim->frames[i]->format, 0, 255, 0)) != 0) {
             fprintf(stderr, "Warning: Color key failed for %s: %s\n", chemin_image, SDL_GetError());
        }
    }
    if (!success) {
        for (int j = 0; j < nb_frames; j++) { if (anim->frames[j]) SDL_FreeSurface(anim->frames[j]); }
        free(anim->frames); free(anim); return NULL;
    }
    anim->frame_actuelle = 0; anim->nb_frames = nb_frames; anim->delai_frame = delai;
    anim->compteur_delai = 0; anim->direction = DIR_RIGHT; // Assume source images face right
    return anim;
}

/* Initialise un ennemi */
Ennemi* initialiser_ennemi(int niveau) {
    (void)niveau; // Mark level as unused for now
    Ennemi *ennemi = (Ennemi*) malloc(sizeof(Ennemi));
    if (!ennemi) { fprintf(stderr, "Erreur alloc ennemi\n"); return NULL; }

    ennemi->position.x = 600; // Start further right
    ennemi->position.y = 300;
    ennemi->position.w = 0; ennemi->position.h = 0; // Set by anim
    ennemi->vitesse_x = 1;  // Slower patrol speed
    ennemi->direction = DIR_LEFT; // Start moving left
    ennemi->health = ENNEMI_HEALTH_MAX;
    ennemi->est_en_attaque = 0; // Start not attacking
    ennemi->temps_changement_direction = 240; // Longer patrol time
    ennemi->compteur_temps = 0;
    ennemi->shoot_timer = 0;
    ennemi->shoot_delay = 90; // Delay between shots (frames) ~1.5s @ 60fps
    ennemi->animations = NULL;

    ennemi->animations = (Animation**) malloc(sizeof(Animation*) * 4);
    if (!ennemi->animations) { fprintf(stderr, "Erreur alloc anim array\n"); free(ennemi); return NULL; }
    for(int i = 0; i < 4; ++i) { ennemi->animations[i] = NULL; }

    // Load RIGHT facing animations
    // IMPORTANT: Ensure these PNGs actually face RIGHT!
    ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT] = initialiser_animation("ennemi_move_right_", 2, 20);
    if (!ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]) { fprintf(stderr, "Failed MOVE_RIGHT anim\n"); liberer_ennemi(ennemi); return NULL; }
    ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT] = initialiser_animation("ennemi_shoot", 1, 30);
    if (!ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT]) { fprintf(stderr, "Failed SHOOT_RIGHT anim\n"); liberer_ennemi(ennemi); return NULL; }

    // For LEFT animations, use the same image data (display func will flip)
    ennemi->animations[ANIM_MOVE * 2 + DIR_LEFT] = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT];
    ennemi->animations[ANIM_SHOOT * 2 + DIR_LEFT] = ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT];

    // Set enemy dimensions based on the first frame
    if (ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]->frames[0]) {
        ennemi->position.w = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]->frames[0]->w;
        ennemi->position.h = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]->frames[0]->h;
    } else {
         fprintf(stderr, "Warning: Could not set enemy dimensions from animation frame.\n");
         // Set default dimensions?
         ennemi->position.w = 32; ennemi->position.h = 32;
    }

    return ennemi;
}


/* Libère l'ennemi */
void liberer_ennemi(Ennemi *ennemi) {
    if (!ennemi) return;
    if (ennemi->animations) {
        // Free only the unique animation sets (RIGHT ones in this setup)
        if (ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]) {
            Animation *anim = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT];
            if (anim->frames) {
                for (int j = 0; j < anim->nb_frames; j++) { if (anim->frames[j]) SDL_FreeSurface(anim->frames[j]); }
                free(anim->frames);
            } free(anim);
        }
        if (ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT] && ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT] != ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]) {
             Animation *anim = ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT];
            if (anim->frames) {
                for (int j = 0; j < anim->nb_frames; j++) { if (anim->frames[j]) SDL_FreeSurface(anim->frames[j]); }
                free(anim->frames);
            } free(anim);
        }
        // Free the array itself
        free(ennemi->animations);
    }
    free(ennemi);
}

/* Affiche l'ennemi */
void afficher_ennemi(Ennemi *ennemi, SDL_Surface *ecran) {
    if (!ennemi || !ecran || !ennemi->animations || ennemi->health <= 0) return;

    SDL_Surface *frame_a_afficher = NULL;
    SDL_Rect dest = ennemi->position;
    int needs_flipping = 0;
    Animation *base_anim = NULL;

    int anim_type = ennemi->est_en_attaque ? ANIM_SHOOT : ANIM_MOVE;
    // Get the source animation data (always use RIGHT source)
    base_anim = ennemi->animations[anim_type * 2 + DIR_RIGHT];

    if (base_anim && base_anim->frames && base_anim->frame_actuelle >= 0 && base_anim->frame_actuelle < base_anim->nb_frames) {
        frame_a_afficher = base_anim->frames[base_anim->frame_actuelle];
    }
    // Fallback to move animation frame 0 if current anim invalid
    else if (!frame_a_afficher && ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT] && ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]->frames && ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]->nb_frames > 0) {
         frame_a_afficher = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]->frames[0];
         if(ennemi->position.w == 0 && frame_a_afficher) { // Ensure dimensions set
            ennemi->position.w = frame_a_afficher->w; ennemi->position.h = frame_a_afficher->h; dest = ennemi->position;
         }
    }

    if (frame_a_afficher) {
        // Flip only if enemy's current direction is LEFT
        needs_flipping = (ennemi->direction == DIR_LEFT);

        // --- Blitting Logic (Manual Flip) ---
        if (!needs_flipping) {
            SDL_BlitSurface(frame_a_afficher, NULL, ecran, &dest);
        } else {
            SDL_Surface *flipped = SDL_CreateRGBSurface(SDL_SWSURFACE, frame_a_afficher->w, frame_a_afficher->h, frame_a_afficher->format->BitsPerPixel,
                                                      frame_a_afficher->format->Rmask, frame_a_afficher->format->Gmask, frame_a_afficher->format->Bmask, frame_a_afficher->format->Amask);
            if (flipped) {
                if (SDL_MUSTLOCK(frame_a_afficher)) SDL_LockSurface(frame_a_afficher);
                if (SDL_MUSTLOCK(flipped)) SDL_LockSurface(flipped);
                int w = frame_a_afficher->w, h = frame_a_afficher->h;
                for (int y = 0; y < h; ++y) { for (int x = 0; x < w; ++x) { put_pixel(flipped, w - 1 - x, y, get_pixel(frame_a_afficher, x, y)); } }
                if (SDL_MUSTLOCK(frame_a_afficher)) SDL_UnlockSurface(frame_a_afficher);
                if (SDL_MUSTLOCK(flipped)) SDL_UnlockSurface(flipped);
                if (frame_a_afficher->flags & SDL_SRCCOLORKEY) { SDL_SetColorKey(flipped, SDL_SRCCOLORKEY, frame_a_afficher->format->colorkey); }
                if (frame_a_afficher->flags & SDL_SRCALPHA) { SDL_SetAlpha(flipped, SDL_SRCALPHA, frame_a_afficher->format->alpha); }
                SDL_BlitSurface(flipped, NULL, ecran, &dest);
                SDL_FreeSurface(flipped);
            } else { SDL_BlitSurface(frame_a_afficher, NULL, ecran, &dest); } // Fallback
        }
        // --- End Blitting Logic ---
    }
}


/* Déplace l'ennemi - MODIFIED AI */
void deplacer_ennemi(Ennemi *ennemi, SDL_Rect limites, SDL_Rect player_pos) {
    if (!ennemi || ennemi->health <= 0) return;

    // --- AI: Check player proximity ---
    int distance_x = abs((player_pos.x + player_pos.w / 2) - (ennemi->position.x + ennemi->position.w / 2));
    int player_is_in_range = (distance_x < ENNEMY_ATTACK_RANGE);

    if (player_is_in_range) {
        if (!ennemi->est_en_attaque) { // Entering attack state
            printf("Player in range! Entering attack state.\n");
            ennemi->est_en_attaque = 1;
            ennemi->shoot_timer = 0; // Reset shoot timer
            // Immediately face the player
             if ((player_pos.x + player_pos.w / 2) < (ennemi->position.x + ennemi->position.w / 2)) {
                 ennemi->direction = DIR_LEFT;
             } else {
                 ennemi->direction = DIR_RIGHT;
             }
             // Reset shoot animation frame
             Animation* shoot_anim = ennemi->animations[ANIM_SHOOT * 2 + DIR_RIGHT]; // Use right source
             if(shoot_anim) shoot_anim->frame_actuelle = 0;
        }
         // Keep facing the player while in range
         if ((player_pos.x + player_pos.w / 2) < (ennemi->position.x + ennemi->position.w / 2)) {
             ennemi->direction = DIR_LEFT;
         } else {
             ennemi->direction = DIR_RIGHT;
         }
        // Enemy stops moving horizontally when attacking
        ennemi->compteur_temps = 0; // Prevent patrol timer from triggering while attacking

    } else { // Player not in range
        if (ennemi->est_en_attaque) { // Exiting attack state
            printf("Player out of range. Resuming patrol.\n");
            ennemi->est_en_attaque = 0;
            ennemi->compteur_temps = 0; // Reset patrol timer
             // Reset move animation frame
             Animation* move_anim = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]; // Use right source
             if(move_anim) move_anim->frame_actuelle = 0;
        }

        // --- Patrolling Logic (only when not attacking) ---
        ennemi->compteur_temps++;
        if (ennemi->compteur_temps >= ennemi->temps_changement_direction) {
            ennemi->compteur_temps = 0;
            // Reverse direction
            ennemi->direction = (ennemi->direction == DIR_LEFT) ? DIR_RIGHT : DIR_LEFT;
             // Reset move animation frame
             Animation* move_anim = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]; // Use right source
             if(move_anim) move_anim->frame_actuelle = 0;
        }

        // Apply horizontal movement only when patrolling
        if (ennemi->direction == DIR_LEFT) {
            ennemi->position.x -= ennemi->vitesse_x;
        } else {
            ennemi->position.x += ennemi->vitesse_x;
        }

        // Check boundaries while patrolling
        if (ennemi->position.x < limites.x) {
            ennemi->position.x = limites.x;
            ennemi->direction = DIR_RIGHT; // Turn around
            ennemi->compteur_temps = 0;    // Reset timer
            Animation* move_anim = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]; if(move_anim) move_anim->frame_actuelle = 0;
        } else if (ennemi->position.x + ennemi->position.w > limites.x + limites.w) {
            ennemi->position.x = limites.x + limites.w - ennemi->position.w;
            ennemi->direction = DIR_LEFT; // Turn around
            ennemi->compteur_temps = 0;   // Reset timer
            Animation* move_anim = ennemi->animations[ANIM_MOVE * 2 + DIR_RIGHT]; if(move_anim) move_anim->frame_actuelle = 0;
        }
    } // End if player_in_range / else
}

/* Anime l'ennemi */
void animer_ennemi(Ennemi *ennemi) {
    if (!ennemi || !ennemi->animations || ennemi->health <= 0) return;

    Animation *anim = NULL;
    int anim_type = ennemi->est_en_attaque ? ANIM_SHOOT : ANIM_MOVE;
    // Get the source animation data (always use RIGHT source)
    anim = ennemi->animations[anim_type * 2 + DIR_RIGHT];

    // If we found a valid animation, update its frame
    if (anim && anim->nb_frames > 0) {
        anim->compteur_delai++;
        if (anim->compteur_delai >= anim->delai_frame) {
            anim->compteur_delai = 0;
            anim->frame_actuelle = (anim->frame_actuelle + 1) % anim->nb_frames;

            // --- REMOVED: Attack state is no longer reset by animation finish ---
            // // If the attack animation just finished its loop
            // if (ennemi->est_en_attaque && anim_type == ANIM_SHOOT && anim->frame_actuelle == 0) {
            //      // Stop attacking after one animation cycle? No, let AI decide.
            // }
        }
    }
}


/* Détecte collision joueur-ennemi */
int detecter_collision_joueur_ennemi(SDL_Rect pos_joueur, Ennemi *ennemi) {
    if (!ennemi || ennemi->health <= 0) return 0;
    if (pos_joueur.x < ennemi->position.x + ennemi->position.w &&
        pos_joueur.x + pos_joueur.w > ennemi->position.x &&
        pos_joueur.y < ennemi->position.y + ennemi->position.h &&
        pos_joueur.y + pos_joueur.h > ennemi->position.y) {
        return 1;
    } return 0;
}

/* Gère santé ennemi */
void gerer_sante_ennemi(Ennemi *ennemi, int dommage) {
    if (!ennemi || ennemi->health <= 0) return;
    ennemi->health -= dommage;
    printf("Enemy hit! Health: %d/%d\n", ennemi->health, ENNEMI_HEALTH_MAX);
    if (ennemi->health <= 0) {
        ennemi->health = 0;
        printf("Enemy defeated!\n");
        ennemi->est_en_attaque = 0; // Stop attacking if dead
    }
}

/* Logique de tir de l'ennemi - MODIFIED for targeting */
void ennemi_shoot(Ennemi *ennemi, Projectile *bullet, SDL_Rect player_pos) {
    if (!ennemi || !bullet || ennemi->health <= 0 || !ennemi->est_en_attaque)
        return; // Only shoot if alive, valid, attacking, and bullet exists

    ennemi->shoot_timer++;

    // Check if ready to shoot AND the bullet is currently inactive
    if (ennemi->shoot_timer >= ennemi->shoot_delay && !bullet->active) {

        // Calculate vector from enemy center to player center
        float enemy_center_x = ennemi->position.x + ennemi->position.w / 2.0f;
        float enemy_center_y = ennemi->position.y + ennemi->position.h / 2.0f;
        float player_center_x = player_pos.x + player_pos.w / 2.0f;
        float player_center_y = player_pos.y + player_pos.h / 2.0f;

        float dx = player_center_x - enemy_center_x;
        float dy = player_center_y - enemy_center_y;

        // Calculate distance
        float distance = sqrt(dx * dx + dy * dy);

        printf("Enemy SHOOTS toward player!\n");

        // Activate and position the bullet
        bullet->active = 1;
        // Start bullet at enemy's center
        bullet->real_x = enemy_center_x - bullet->pos.w / 2.0f;
        bullet->real_y = enemy_center_y - bullet->pos.h / 2.0f;
        bullet->pos.x = (int)bullet->real_x;
        bullet->pos.y = (int)bullet->real_y;


        // Calculate normalized direction vector and set velocity
        if (distance > 0) { // Avoid division by zero
            float norm_x = dx / distance;
            float norm_y = dy / distance;
            bullet->vx = norm_x * ENNEMY_BULLET_SPEED;
            bullet->vy = norm_y * ENNEMY_BULLET_SPEED;
        } else {
            // Player is exactly on top? Fire horizontally based on facing direction
            bullet->vx = (ennemi->direction == DIR_LEFT) ? -ENNEMY_BULLET_SPEED : ENNEMY_BULLET_SPEED;
            bullet->vy = 0;
        }

        ennemi->shoot_timer = 0; // Reset timer after shooting
    }
}
