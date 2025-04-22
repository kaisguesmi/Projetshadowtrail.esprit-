/*
main.c - Exemple d'utilisation du module ennemi with targeted shooting
*/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>   // Include math here too just in case
#include "ennemi.h" // Includes Projectile struct now

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 32

#define BULLET_IMAGE_FILE "enemy_bullet.png" // Make sure this exists!

int main(int argc, char *argv[]) {
    (void)argc; (void)argv; // Mark as unused

    SDL_Surface *ecran = NULL;
    SDL_Event event;
    int continuer = 1;

    if (SDL_Init(SDL_INIT_VIDEO) == -1) { fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError()); return EXIT_FAILURE; }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) { fprintf(stderr, "IMG Init Error: %s\n", IMG_GetError()); SDL_Quit(); return EXIT_FAILURE; }

    ecran = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!ecran) { fprintf(stderr, "Video Mode Error: %s\n", SDL_GetError()); IMG_Quit(); SDL_Quit(); return EXIT_FAILURE; }
    SDL_WM_SetCaption("Module Ennemi - Targeted Shooting Demo", NULL);
    srand(time(NULL));

    SDL_Rect limites_jeu = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // --- Player Setup ---
    SDL_Rect pos_joueur = {100, SCREEN_HEIGHT / 2, 32, 32};
    SDL_Surface *image_joueur = IMG_Load("joueur.png");
    if (!image_joueur) { fprintf(stderr, "Load Player Error: %s\n", IMG_GetError()); IMG_Quit(); SDL_Quit(); return EXIT_FAILURE; }
    if (SDL_SetColorKey(image_joueur, SDL_SRCCOLORKEY, SDL_MapRGB(image_joueur->format, 0, 255, 0)) != 0) { fprintf(stderr, "Player Color Key Warn: %s\n", SDL_GetError()); }
    pos_joueur.w = image_joueur->w; pos_joueur.h = image_joueur->h;

    // --- Hearts Setup ---
    SDL_Surface *heart_full = IMG_Load("heart_full.png");
    SDL_Surface *heart_empty = IMG_Load("heart_empty.png");
    if (!heart_full || !heart_empty) { fprintf(stderr, "Load Heart Error: %s\n", IMG_GetError()); SDL_FreeSurface(image_joueur); IMG_Quit(); SDL_Quit(); return EXIT_FAILURE; }
    if (SDL_SetColorKey(heart_full, SDL_SRCCOLORKEY, SDL_MapRGB(heart_full->format, 0, 0, 0)) != 0) { fprintf(stderr, "Heart Full Key Warn: %s\n", SDL_GetError());}
    if (SDL_SetColorKey(heart_empty, SDL_SRCCOLORKEY, SDL_MapRGB(heart_empty->format, 0, 0, 0)) != 0) { fprintf(stderr, "Heart Empty Key Warn: %s\n", SDL_GetError());}

    // --- Bullet Setup ---
    SDL_Surface *image_bullet = IMG_Load(BULLET_IMAGE_FILE);
    if (!image_bullet) { fprintf(stderr, "Load Bullet Error '%s': %s\n", BULLET_IMAGE_FILE, IMG_GetError()); SDL_FreeSurface(heart_full); SDL_FreeSurface(heart_empty); SDL_FreeSurface(image_joueur); IMG_Quit(); SDL_Quit(); return EXIT_FAILURE; }
    // Optional: Set color key for bullet
    // SDL_SetColorKey(image_bullet, SDL_SRCCOLORKEY, SDL_MapRGB(image_bullet->format, R, G, B));

    Projectile enemy_bullet;
    enemy_bullet.active = 0;
    enemy_bullet.vx = 0.0f; enemy_bullet.vy = 0.0f;
    enemy_bullet.image = image_bullet;
    enemy_bullet.pos.w = image_bullet->w; enemy_bullet.pos.h = image_bullet->h;
    enemy_bullet.real_x = 0.0f; enemy_bullet.real_y = 0.0f; // Initialize real coords
    enemy_bullet.pos.x = 0; enemy_bullet.pos.y = 0;

    // --- Enemy Setup ---
    Ennemi *ennemi = initialiser_ennemi(1);
    if (!ennemi) { fprintf(stderr, "Init Enemy Error\n"); SDL_FreeSurface(image_bullet); SDL_FreeSurface(heart_full); SDL_FreeSurface(heart_empty); SDL_FreeSurface(image_joueur); IMG_Quit(); SDL_Quit(); return EXIT_FAILURE; }


    // --- Main Loop ---
    while (continuer) {
        // --- Event Handling ---
        while (SDL_PollEvent(&event)) {
             switch (event.type) {
                 case SDL_QUIT: continuer = 0; break;
                 case SDL_KEYDOWN:
                     switch (event.key.keysym.sym) {
                         case SDLK_ESCAPE: continuer = 0; break;
                         case SDLK_UP:    pos_joueur.y -= 5; break;
                         case SDLK_DOWN:  pos_joueur.y += 5; break;
                         case SDLK_LEFT:  pos_joueur.x -= 5; break;
                         case SDLK_RIGHT: pos_joueur.x += 5; break;
                         case SDLK_SPACE: gerer_sante_ennemi(ennemi, 1); break; // Damage enemy
                         default: break;
                     } break;
             }
        }

        // --- Player Bounds Check ---
         if (pos_joueur.x < 0) pos_joueur.x = 0;
         if (pos_joueur.y < 0) pos_joueur.y = 0;
         if (pos_joueur.x > SCREEN_WIDTH - pos_joueur.w) pos_joueur.x = SCREEN_WIDTH - pos_joueur.w;
         if (pos_joueur.y > SCREEN_HEIGHT - pos_joueur.h) pos_joueur.y = SCREEN_HEIGHT - pos_joueur.h;


        // --- Game Logic Updates ---

        // Player vs Enemy Collision
        if (ennemi && detecter_collision_joueur_ennemi(pos_joueur, ennemi)) {
            printf("Collision avec l'ennemi!\n"); // Handle player damage here
        }

        // Enemy actions (only if enemy is alive)
        if (ennemi && ennemi->health > 0) {
            // Pass player position to enemy AI functions
            deplacer_ennemi(ennemi, limites_jeu, pos_joueur);
            animer_ennemi(ennemi);
            ennemi_shoot(ennemi, &enemy_bullet, pos_joueur);
        }

        // Update Bullet
        if (enemy_bullet.active) {
            // Update real floating point position
            enemy_bullet.real_x += enemy_bullet.vx;
            enemy_bullet.real_y += enemy_bullet.vy;
            // Update integer rect for drawing/collision
            enemy_bullet.pos.x = (int)enemy_bullet.real_x;
            enemy_bullet.pos.y = (int)enemy_bullet.real_y;

            // Deactivate bullet if it goes off screen
            if (enemy_bullet.pos.x + enemy_bullet.pos.w < 0 || enemy_bullet.pos.x > SCREEN_WIDTH ||
                enemy_bullet.pos.y + enemy_bullet.pos.h < 0 || enemy_bullet.pos.y > SCREEN_HEIGHT) {
                enemy_bullet.active = 0;
            }

            // Check Bullet vs Player Collision
            else if (pos_joueur.x < enemy_bullet.pos.x + enemy_bullet.pos.w &&
                     pos_joueur.x + pos_joueur.w > enemy_bullet.pos.x &&
                     pos_joueur.y < enemy_bullet.pos.y + enemy_bullet.pos.h &&
                     pos_joueur.y + pos_joueur.h > enemy_bullet.pos.y) {
                printf("PLAYER HIT by bullet!\n");
                enemy_bullet.active = 0; // Deactivate bullet on hit
                // Apply damage to player here...
            }
        }


        // --- Rendering ---
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0)); // Clear screen

        if (image_joueur) { SDL_BlitSurface(image_joueur, NULL, ecran, &pos_joueur); } // Draw Player

        if (ennemi && ennemi->health > 0) { afficher_ennemi(ennemi, ecran); } // Draw Enemy

        if (enemy_bullet.active && enemy_bullet.image) { SDL_BlitSurface(enemy_bullet.image, NULL, ecran, &enemy_bullet.pos); } // Draw Bullet

        // Draw Health Bar
        if (heart_full && heart_empty && ennemi) {
            int hx = 10, hy = 10, hw = heart_full->w;
            for (int i = 0; i < ENNEMI_HEALTH_MAX; i++) {
                SDL_Surface *h_img = (i < ennemi->health) ? heart_full : heart_empty;
                SDL_Rect h_pos = {hx, hy, 0, 0};
                SDL_BlitSurface(h_img, NULL, ecran, &h_pos);
                hx += hw + 5;
            }
        }

        if (SDL_Flip(ecran) == -1) { fprintf(stderr, "Flip Error: %s\n", SDL_GetError()); continuer = 0; } // Update screen

        SDL_Delay(16); // Frame rate cap (~60 FPS)

    } // End main loop

    // --- Cleanup ---
    printf("Exiting...\n");
    liberer_ennemi(ennemi);
    SDL_FreeSurface(image_bullet);
    SDL_FreeSurface(image_joueur);
    SDL_FreeSurface(heart_full);
    SDL_FreeSurface(heart_empty);
    IMG_Quit();
    SDL_Quit();
    printf("Cleanup complete.\n");
    return EXIT_SUCCESS;
}
