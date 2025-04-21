#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL/SDL.h>

// Constants for obstacle types
#define OBSTACLE_TYPE_FIXED 0
#define OBSTACLE_TYPE_HOLE 1
#define OBSTACLE_TYPE_DESTRUCTIBLE 2

// Number of flash colors for minimap obstacles
#define NUM_OBSTACLE_FLASH_COLORS 4

// --- Core Game Element Structs ---

typedef struct {
    SDL_Rect position;
    int vie;
    int score;
    int vx;
    int vy;
} Personnage;

typedef struct {
    SDL_Rect camera;    // Represents the viewport into the world
    SDL_Surface *image; // Pointer to the *loaded* main background surface
    int world_width;
    int world_height;
} Background;

typedef struct {
    SDL_Rect position;
    int type;
} Obstacle;

typedef struct {
    SDL_Rect position;
    int type;
    int direction;
    SDL_Surface* image; // Main screen visual (not used by minimap logic)
    int active;
} Ennemi;

// --- Minimap Specific Struct ---
typedef struct {
    SDL_Surface *image;       // Minimap background surface
    SDL_Rect position;        // Position on screen
    SDL_Surface *joueurImage; // Icon surface
    SDL_Rect joueurPosition;  // Relative position on minimap surface
} minimap;


// --- Function Prototypes ---

// Utility / Collision
int estEnCollision(SDL_Rect a, SDL_Rect b);

// Camera Update
void mettreAJourCamera(Background *bg, const Personnage *joueur, int screenWidth); // Added prototype

// Level Loading
int chargerNiveau(int levelNum, Background *bg, minimap *m, Personnage *j,
                   Ennemi ennemis[], int *numEnnemis,
                   Obstacle obstacles[], int *numObstacles,
                   float *minimapScale,
                   const char* minimapBgPathL1, const char* minimapBgPathL2,
                   const char* mainBgPathL1, const char* mainBgPathL2, // Added main bg paths
                   int screenWidth, int screenHeight, // Added screenHeight back
                   int minimapOffsetX, int minimapOffsetY,
                   SDL_Surface* playerSprite, SDL_Surface* enemySprite);

// Rendering
void afficherBackground(SDL_Surface *screen, const Background *bg); // Added prototype
void afficherMinimap(SDL_Surface *screen, minimap m,
                      const Obstacle obstacles[], int numObstacles,
                      const Ennemi ennemis[], int numEnnemis,
                      float redimensionnement,
                      int currentObstacleColorIndex,
                      int isPlayerShaking);

// Asset Management
int initialiserMinimapAssets(minimap *m, const char *minimapPlayerIconPath);
void libererMinimapAssets(minimap *m);


#endif // MINIMAP_H
