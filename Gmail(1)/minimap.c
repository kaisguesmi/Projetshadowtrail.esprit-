#include "minimap.h"
#include <stdio.h>
#include <stdlib.h> // For rand()
#include <SDL/SDL_image.h> // Include SDL_image

// Define obstacle flashing colors
static const SDL_Color OBSTACLE_FLASH_COLORS[] = {
    {.r = 255, .g = 0, .b = 0},   // Red
    {.r = 255, .g = 165, .b = 0}, // Orange
    {.r = 255, .g = 255, .b = 0}, // Yellow
    {.r = 165, .g = 0, .b = 255}  // Purple
};
// NUM_OBSTACLE_FLASH_COLORS is defined in minimap.h

static const int MINIMAP_ENEMY_DOT_SIZE = 5;
static const int MINIMAP_SHAKE_AMPLITUDE = 3;

// --- Function Implementations ---

int initialiserMinimapAssets(minimap *m, const char *minimapPlayerIconPath) {
    // ... (implementation as before) ...
     if (!m || !minimapPlayerIconPath) return 0;
    printf("Initializing Minimap Assets (Player Icon only)...\n");

    m->image = NULL;
    m->joueurImage = NULL;
    m->position = (SDL_Rect){0, 0, 0, 0};
    m->joueurPosition = (SDL_Rect){0, 0, 0, 0};

    m->joueurImage = IMG_Load(minimapPlayerIconPath);
    if (!m->joueurImage) {
        fprintf(stderr, "ERROR loading minimap player icon '%s': %s\n", minimapPlayerIconPath, IMG_GetError());
        return 0;
    }
    printf("Minimap player icon loaded.\n");
    return 1;
}


void MAJMinimap(SDL_Rect posJoueurAbs, minimap *m, float redimensionnement) {
    // Updates the 'joueurPosition' relative to the top-left of the minimap surface
    if (!m || !m->joueurImage || redimensionnement <= 0) return;

    // Center the player world position before scaling
    float player_center_x = posJoueurAbs.x + posJoueurAbs.w / 2.0f;
    // *** FIX THE TYPO HERE ***
    float player_center_y = posJoueurAbs.y + posJoueurAbs.h / 2.0f;

    // Calculate position RELATIVE to the minimap's top-left corner
    m->joueurPosition.x = (int)(player_center_x * redimensionnement);
    m->joueurPosition.y = (int)(player_center_y * redimensionnement);

    // Adjust position so the icon is centered on the calculated point
    m->joueurPosition.x -= m->joueurImage->w / 2;
    m->joueurPosition.y -= m->joueurImage->h / 2;

    // Set icon size (mostly for completeness)
    m->joueurPosition.w = m->joueurImage->w;
    m->joueurPosition.h = m->joueurImage->h;
}


void afficherMinimap(SDL_Surface *screen, minimap m,
                      const Obstacle obstacles[], int numObstacles,
                      const Ennemi ennemis[], int numEnnemis,
                      float redimensionnement,
                      int currentObstacleColorIndex,
                      int isPlayerShaking)
{
    // ... (implementation as before) ...
     if (!screen || !m.image || !m.joueurImage || redimensionnement <= 0) {
        return;
    }

    SDL_Rect previous_clip;
    SDL_GetClipRect(screen, &previous_clip);
    SDL_SetClipRect(screen, &m.position);

    SDL_BlitSurface(m.image, NULL, screen, &m.position);

    if (currentObstacleColorIndex < 0 || currentObstacleColorIndex >= NUM_OBSTACLE_FLASH_COLORS) {
         currentObstacleColorIndex = 0;
    }
    SDL_Color flashColor = OBSTACLE_FLASH_COLORS[currentObstacleColorIndex];
    Uint32 mappedFlashColor = SDL_MapRGB(screen->format, flashColor.r, flashColor.g, flashColor.b);

    for (int i = 0; i < numObstacles; i++) {
        if (obstacles[i].type != OBSTACLE_TYPE_HOLE)
        {
            SDL_Rect obstacle_rect_minimap;
            obstacle_rect_minimap.x = (int)(obstacles[i].position.x * redimensionnement);
            obstacle_rect_minimap.y = (int)(obstacles[i].position.y * redimensionnement);
            obstacle_rect_minimap.w = (int)(obstacles[i].position.w * redimensionnement);
            obstacle_rect_minimap.h = (int)(obstacles[i].position.h * redimensionnement);

            if (obstacle_rect_minimap.w < 2) obstacle_rect_minimap.w = 2;
            if (obstacle_rect_minimap.h < 2) obstacle_rect_minimap.h = 2;

            obstacle_rect_minimap.x += m.position.x;
            obstacle_rect_minimap.y += m.position.y;

            SDL_FillRect(screen, &obstacle_rect_minimap, mappedFlashColor);
        }
    }

    Uint32 enemyColor = SDL_MapRGB(screen->format, 255, 0, 0);

    for (int i = 0; i < numEnnemis; i++) {
        if (ennemis[i].active) {
             float enemy_center_x = ennemis[i].position.x + ennemis[i].position.w / 2.0f;
             float enemy_center_y = ennemis[i].position.y + ennemis[i].position.h / 2.0f;

            SDL_Rect enemy_dot;
            enemy_dot.x = (int)(enemy_center_x * redimensionnement);
            enemy_dot.y = (int)(enemy_center_y * redimensionnement);
            enemy_dot.w = MINIMAP_ENEMY_DOT_SIZE;
            enemy_dot.h = MINIMAP_ENEMY_DOT_SIZE;

            enemy_dot.x += m.position.x - (enemy_dot.w / 2);
            enemy_dot.y += m.position.y - (enemy_dot.h / 2);

             SDL_FillRect(screen, &enemy_dot, enemyColor);
        }
    }

    SDL_Rect player_draw_pos = m.joueurPosition;

    if (isPlayerShaking) {
         player_draw_pos.x += (rand() % (MINIMAP_SHAKE_AMPLITUDE * 2 + 1)) - MINIMAP_SHAKE_AMPLITUDE;
         player_draw_pos.y += (rand() % (MINIMAP_SHAKE_AMPLITUDE * 2 + 1)) - MINIMAP_SHAKE_AMPLITUDE;
    }

    player_draw_pos.x += m.position.x;
    player_draw_pos.y += m.position.y;

    SDL_BlitSurface(m.joueurImage, NULL, screen, &player_draw_pos);

    SDL_SetClipRect(screen, &previous_clip);
}


void libererMinimapAssets(minimap *m) {
   // ... (implementation as before) ...
    if (!m) return;
    printf("Freeing Minimap Assets...\n");

    if (m->joueurImage) {
        SDL_FreeSurface(m->joueurImage);
        m->joueurImage = NULL;
    }
    if (m->image) {
        SDL_FreeSurface(m->image);
        m->image = NULL;
    }
    printf("Minimap assets freed.\n");
}

int estEnCollision(SDL_Rect a, SDL_Rect b) {
   // ... (implementation as before) ...
    if (a.x + a.w <= b.x) return 0;
    if (a.x >= b.x + b.w) return 0;
    if (a.y + a.h <= b.y) return 0;
    if (a.y >= b.y + b.h) return 0;
    return 1;
}
