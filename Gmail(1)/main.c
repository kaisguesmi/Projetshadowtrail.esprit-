#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>

#include "minimap.h" // Includes declarations

// --- Constants for Test ---
#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       960
#define WORLD_WIDTH_L1      8000
#define WORLD_HEIGHT_L1     1080
#define WORLD_WIDTH_L2      8000
#define WORLD_HEIGHT_L2     1080
#define MAX_OBSTACLES       10
#define MAX_ENNEMIS         5
#define PLAYER_MOVE_SPEED   6
#define ENEMY_MOVE_SPEED    2
#define CAMERA_FOLLOW_MARGIN_X 400
#define CAMERA_FOLLOW_MARGIN_Y 200
#define MINIMAP_SCREEN_X_OFFSET 20
#define MINIMAP_SCREEN_Y_OFFSET 20
#define MINIMAP_PLAYER_ICON_PATH "joueur_minimap.png"
#define MINIMAP_BG_L1_PATH       "minimap_bg_scaled_L1.png"
#define MINIMAP_BG_L2_PATH       "minimap_bg_scaled_L2.png"
#define MINIMAP_FLASH_INTERVAL   200
#define MAIN_BG_L1_PATH          "bg1.png"
#define MAIN_BG_L2_PATH          "bg2.png"


// --- Forward declarations ---
int chargerNiveau(int levelNum, Background *bg, minimap *m, Personnage *j,
                   Ennemi ennemis[], int *numEnnemis,
                   Obstacle obstacles[], int *numObstacles,
                   float *minimapScale,
                   const char* minimapBgPathL1, const char* minimapBgPathL2,
                   const char* mainBgPathL1, const char* mainBgPathL2,
                   int screenWidth, int screenHeight,
                   int minimapOffsetX, int minimapOffsetY,
                   SDL_Surface* playerSprite, SDL_Surface* enemySprite);
void mettreAJourCamera(Background *bg, const Personnage *joueur, int screenWidth);
void afficherBackground(SDL_Surface *screen, const Background *bg);
void MAJMinimap(SDL_Rect posJoueurAbs, minimap *m, float redimensionnement);


// --- Main Function ---
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    // SDL Initialization
    // ... (Error handling as before) ...
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) { fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError()); return 1; }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) { fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError()); SDL_Quit(); return 1; }
    if (TTF_Init() == -1) { fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError()); IMG_Quit(); SDL_Quit(); return 1; }

    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) { fprintf(stderr, "SetVideoMode Error: %s\n", SDL_GetError()); TTF_Quit(); IMG_Quit(); SDL_Quit(); return 1; }
    SDL_WM_SetCaption("Minimap Test with Scrolling Background", NULL);
    srand(time(NULL));

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) { fprintf(stderr, "Warning: Failed to load font 'arial.ttf': %s\n", TTF_GetError()); }

    // Game Object Initialization
    Personnage personnage;
    Background background_obj = {0};
    minimap minimap_data = {0};
    Obstacle obstacles[MAX_OBSTACLES];
    Ennemi ennemis[MAX_ENNEMIS];
    int numObstacles = 0;
    int numEnnemis = 0;
    int current_level = 1;

    float minimap_scale_factor = 1.0f;
    Uint32 minimap_flash_timer = 0;
    int minimap_flash_index = 0;
    int player_is_shaking = 0;

    // Load Essential Sprites
    SDL_Surface* player_sprite_main = IMG_Load("player_sprite.png");
    if (!player_sprite_main) { /* Error Handling */ return 1; }
    SDL_Surface* enemy_sprite_main = IMG_Load("enemy_sprite.png");
    if (!enemy_sprite_main) { /* Warning */ }

    // Initialize Minimap Player Icon
    if (!initialiserMinimapAssets(&minimap_data, MINIMAP_PLAYER_ICON_PATH)) { /* Error Handling */ return 1; }

    // Load Initial Level Data
    if (!chargerNiveau(current_level, &background_obj, &minimap_data, &personnage,
                       ennemis, &numEnnemis, obstacles, &numObstacles,
                       &minimap_scale_factor,
                       MINIMAP_BG_L1_PATH, MINIMAP_BG_L2_PATH,
                       MAIN_BG_L1_PATH, MAIN_BG_L2_PATH,
                       SCREEN_WIDTH, SCREEN_HEIGHT,
                       MINIMAP_SCREEN_X_OFFSET, MINIMAP_SCREEN_Y_OFFSET,
                       player_sprite_main, enemy_sprite_main))
    { /* Error Handling */ return 1; }

    // --- Main Loop ---
    int running = 1;
    // *** REMOVED unused variables ***
    // SDL_Event event;
    // Uint8 *keystate = NULL;

    printf("\n--- Controls ---\n");
    printf("Arrow Keys: Move Player\n");
    printf("L: Switch Level (1 <-> 2)\n");
    printf("ESC: Quit\n");
    printf("-----------------\n");

    while (running) {
        Uint32 current_time = SDL_GetTicks();

        // Event Handling (Simplified for this test - only QUIT and keydown)
        SDL_Event event_polled; // Use a temporary variable inside the loop
        while (SDL_PollEvent(&event_polled)) {
            if (event_polled.type == SDL_QUIT) { running = 0; }
            else if (event_polled.type == SDL_KEYDOWN) {
                switch (event_polled.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_l:
                        current_level = (current_level == 1) ? 2 : 1;
                        printf("Switching to Level %d\n", current_level);
                        if (!chargerNiveau(current_level, &background_obj, &minimap_data, &personnage,
                                           ennemis, &numEnnemis, obstacles, &numObstacles,
                                           &minimap_scale_factor,
                                           MINIMAP_BG_L1_PATH, MINIMAP_BG_L2_PATH,
                                           MAIN_BG_L1_PATH, MAIN_BG_L2_PATH,
                                           SCREEN_WIDTH, SCREEN_HEIGHT,
                                           MINIMAP_SCREEN_X_OFFSET, MINIMAP_SCREEN_Y_OFFSET,
                                           player_sprite_main, enemy_sprite_main))
                         { running = 0; }
                        break;
                    default: break;
                }
            }
        }

        // Input Processing (Direct state reading)
        Uint8 *key_state_now = SDL_GetKeyState(NULL); // Get current state inside loop
        personnage.vx = 0; personnage.vy = 0;
        if (key_state_now[SDLK_LEFT]) personnage.vx = -PLAYER_MOVE_SPEED;
        if (key_state_now[SDLK_RIGHT]) personnage.vx = PLAYER_MOVE_SPEED;
        if (key_state_now[SDLK_UP]) personnage.vy = -PLAYER_MOVE_SPEED;
        if (key_state_now[SDLK_DOWN]) personnage.vy = PLAYER_MOVE_SPEED;

        // Game Logic Update
        player_is_shaking = 0;
        personnage.position.x += personnage.vx;
        personnage.position.y += personnage.vy;
        // ... (Clamping, Collision, Enemy Update, Camera Update, Minimap Update, Flash Timer Update) ...
         // Clamp player
        if (personnage.position.x < 0) personnage.position.x = 0;
        if (personnage.position.x + personnage.position.w > background_obj.world_width) personnage.position.x = background_obj.world_width - personnage.position.w;
        if (personnage.position.y < 0) personnage.position.y = 0;
        if (personnage.position.y + personnage.position.h > background_obj.world_height) personnage.position.y = background_obj.world_height - personnage.position.h;

        // Collision
        for (int i = 0; i < numObstacles; ++i) {
            if (obstacles[i].type != OBSTACLE_TYPE_HOLE && estEnCollision(personnage.position, obstacles[i].position)) {
                player_is_shaking = 1;
                // Simple Response
                if (personnage.vx > 0) personnage.position.x = obstacles[i].position.x - personnage.position.w - 1;
                else if (personnage.vx < 0) personnage.position.x = obstacles[i].position.x + obstacles[i].position.w + 1;
                personnage.vx = 0;
                if (personnage.vy > 0) personnage.position.y = obstacles[i].position.y - personnage.position.h - 1;
                else if (personnage.vy < 0) personnage.position.y = obstacles[i].position.y + obstacles[i].position.h + 1;
                personnage.vy = 0;
                // Re-clamp
                 if (personnage.position.x < 0) personnage.position.x = 0;
                 if (personnage.position.x + personnage.position.w > background_obj.world_width) personnage.position.x = background_obj.world_width - personnage.position.w;
                 if (personnage.position.y < 0) personnage.position.y = 0;
                 if (personnage.position.y + personnage.position.h > background_obj.world_height) personnage.position.y = background_obj.world_height - personnage.position.h;
            }
        }
        // Update enemies
        for (int i = 0; i < numEnnemis; i++) {
             if (ennemis[i].active) {
                ennemis[i].position.x += ennemis[i].direction * ENEMY_MOVE_SPEED;
                if (ennemis[i].position.x < 100 || ennemis[i].position.x + ennemis[i].position.w > background_obj.world_width - 100) {
                    ennemis[i].direction *= -1;
                    ennemis[i].position.x += ennemis[i].direction * ENEMY_MOVE_SPEED * 2;
                }
            }
        }
        // Update Camera
        mettreAJourCamera(&background_obj, &personnage, SCREEN_WIDTH);
        // Update Minimap
        MAJMinimap(personnage.position, &minimap_data, minimap_scale_factor);
        // Update flash timer
        if (current_time - minimap_flash_timer >= MINIMAP_FLASH_INTERVAL) {
            minimap_flash_timer = current_time;
            minimap_flash_index = (minimap_flash_index + 1) % NUM_OBSTACLE_FLASH_COLORS;
        }


        // Rendering
        // ... (Render background, obstacles, enemies, player, minimap, HUD) ...
        afficherBackground(screen, &background_obj);
        Uint32 obs_color = SDL_MapRGB(screen->format, 100, 100, 100);
        for (int i = 0; i < numObstacles; ++i) {
             if (obstacles[i].type != OBSTACLE_TYPE_HOLE) {
                 SDL_Rect draw_pos = obstacles[i].position;
                 draw_pos.x -= background_obj.camera.x; draw_pos.y -= background_obj.camera.y;
                 if (draw_pos.x + draw_pos.w > 0 && draw_pos.x < SCREEN_WIDTH && draw_pos.y + draw_pos.h > 0 && draw_pos.y < SCREEN_HEIGHT)
                      SDL_FillRect(screen, &draw_pos, obs_color);
             }
        }
        for (int i = 0; i < numEnnemis; ++i) {
            if (ennemis[i].active && enemy_sprite_main) {
                 SDL_Rect draw_pos = ennemis[i].position;
                 draw_pos.x -= background_obj.camera.x; draw_pos.y -= background_obj.camera.y;
                 if (draw_pos.x + draw_pos.w > 0 && draw_pos.x < SCREEN_WIDTH && draw_pos.y + draw_pos.h > 0 && draw_pos.y < SCREEN_HEIGHT)
                    SDL_BlitSurface(enemy_sprite_main, NULL, screen, &draw_pos);
            }
        }
        if (player_sprite_main) {
             SDL_Rect player_draw_pos = personnage.position;
             player_draw_pos.x -= background_obj.camera.x; player_draw_pos.y -= background_obj.camera.y;
             if (player_draw_pos.x + player_draw_pos.w > 0 && player_draw_pos.x < SCREEN_WIDTH && player_draw_pos.y + player_draw_pos.h > 0 && player_draw_pos.y < SCREEN_HEIGHT)
                SDL_BlitSurface(player_sprite_main, NULL, screen, &player_draw_pos);
        }
        afficherMinimap(screen, minimap_data, obstacles, numObstacles, ennemis, numEnnemis, minimap_scale_factor, minimap_flash_index, player_is_shaking);
        if (font) {
             char level_text[20];
             snprintf(level_text, sizeof(level_text), "Level: %d", current_level);
             SDL_Color white = {.r = 255, .g = 255, .b = 255};
             SDL_Surface* text_surface = TTF_RenderText_Solid(font, level_text, white);
             if (text_surface) {
                 SDL_Rect text_pos = {10, 10, 0, 0};
                 SDL_BlitSurface(text_surface, NULL, screen, &text_pos);
                 SDL_FreeSurface(text_surface);
             }
        }


        // Flip Screen & Delay
        if (SDL_Flip(screen) == -1) { /* Error Handling */ running = 0; }
        SDL_Delay(16);
    }

    // Cleanup
    // ... (Cleanup remains the same) ...
    printf("Cleaning up...\n");
    libererMinimapAssets(&minimap_data);
    if (background_obj.image) SDL_FreeSurface(background_obj.image);
    if (player_sprite_main) SDL_FreeSurface(player_sprite_main);
    if (enemy_sprite_main) SDL_FreeSurface(enemy_sprite_main);
    if (font) TTF_CloseFont(font);
    TTF_Quit(); IMG_Quit(); SDL_Quit();
    printf("Done.\n");

    return 0;
}


// --- Helper Function Implementations ---
// ... (chargerNiveau, mettreAJourCamera, afficherBackground implementations remain the same) ...
int chargerNiveau(int levelNum, Background *bg, minimap *m, Personnage *j,
                   Ennemi ennemis[], int *numEnnemis,
                   Obstacle obstacles[], int *numObstacles,
                   float *minimapScale,
                   const char* minimapBgPathL1, const char* minimapBgPathL2,
                   const char* mainBgPathL1, const char* mainBgPathL2,
                   int screenWidth, int screenHeight,
                   int minimapOffsetX, int minimapOffsetY,
                   SDL_Surface* playerSprite, SDL_Surface* enemySprite)
{
    printf("Loading Level %d...\n", levelNum);
    *numObstacles = 0; *numEnnemis = 0;

    if (bg->image) { SDL_FreeSurface(bg->image); bg->image = NULL; }
    if (m->image) { SDL_FreeSurface(m->image); m->image = NULL; }

    const char* minimapBgPath = NULL;
    const char* mainBgPath = NULL;

    if (levelNum == 1) {
        bg->world_width = WORLD_WIDTH_L1; bg->world_height = WORLD_HEIGHT_L1;
        minimapBgPath = minimapBgPathL1; mainBgPath = mainBgPathL1;
        obstacles[0] = (Obstacle){{1150, 665, 200, 50}, OBSTACLE_TYPE_FIXED};
        obstacles[1] = (Obstacle){{3400, 670, 150, 50}, OBSTACLE_TYPE_FIXED};
        obstacles[2] = (Obstacle){{2000, 150, 80, 80}, OBSTACLE_TYPE_HOLE};
        obstacles[3] = (Obstacle){{5500, 540, 180, 60}, OBSTACLE_TYPE_FIXED};
        obstacles[4] = (Obstacle){{7550, 330, 250, 70}, OBSTACLE_TYPE_FIXED};
        *numObstacles = 5;
        if (MAX_ENNEMIS >= 2) {
             ennemis[0] = (Ennemi){{6550, bg->world_height - 100, 60, 80}, 0, 1, NULL, 1};
             ennemis[1] = (Ennemi){{3000, bg->world_height - 100, 60, 80}, 0,-1, NULL, 1};
             *numEnnemis = 2;
             if(enemySprite) {
                ennemis[0].position.w=enemySprite->w; ennemis[0].position.h=enemySprite->h; ennemis[0].position.y = bg->world_height - ennemis[0].position.h;
                ennemis[1].position.w=enemySprite->w; ennemis[1].position.h=enemySprite->h; ennemis[1].position.y = bg->world_height - ennemis[1].position.h;
             }
        }
    } else if (levelNum == 2) {
        bg->world_width = WORLD_WIDTH_L2; bg->world_height = WORLD_HEIGHT_L2;
        minimapBgPath = minimapBgPathL2; mainBgPath = mainBgPathL2;
         obstacles[0] = (Obstacle){{1300, 625, 180, 40}, OBSTACLE_TYPE_FIXED};
         obstacles[1] = (Obstacle){{3800, 530, 220, 60}, OBSTACLE_TYPE_FIXED};
         obstacles[2] = (Obstacle){{710, 830, 100, 30}, OBSTACLE_TYPE_FIXED};
         obstacles[3] = (Obstacle){{2400, 680, 50, 150}, OBSTACLE_TYPE_FIXED};
         obstacles[4] = (Obstacle){{5520, 600, 120, 120}, OBSTACLE_TYPE_HOLE};
         *numObstacles = 5;
         if (MAX_ENNEMIS >= 1) {
             ennemis[0] = (Ennemi){{7500, bg->world_height - 80, 60, 80}, 1, -1, NULL, 1};
             *numEnnemis = 1;
             if(enemySprite) {
                 ennemis[0].position.w=enemySprite->w; ennemis[0].position.h=enemySprite->h; ennemis[0].position.y = bg->world_height - ennemis[0].position.h;
             }
         }
    } else { return 0; }

    if (!mainBgPath) return 0;
    bg->image = IMG_Load(mainBgPath);
    if (!bg->image) { fprintf(stderr, "ERROR loading main background '%s': %s\n", mainBgPath, IMG_GetError()); return 0; }

    if (!minimapBgPath) { if(bg->image) SDL_FreeSurface(bg->image); bg->image = NULL; return 0; }
    m->image = IMG_Load(minimapBgPath);
    if (!m->image) { fprintf(stderr, "ERROR loading minimap background '%s': %s\n", minimapBgPath, IMG_GetError()); if(bg->image) SDL_FreeSurface(bg->image); bg->image = NULL; return 0; }

    m->position.w = m->image->w; m->position.h = m->image->h;
    m->position.x = screenWidth - m->position.w - minimapOffsetX;
    m->position.y = minimapOffsetY;
    if (bg->world_width > 0 && m->image->w > 0) { *minimapScale = (float)m->image->w / (float)bg->world_width; }
    else { *minimapScale = 1.0f; }
    printf("Minimap loaded (Scale: %.4f).\n", *minimapScale);

    if (playerSprite) { j->position.w = playerSprite->w; j->position.h = playerSprite->h; }
    else { j->position.w = 30; j->position.h = 50; }
    j->position.x = 100; j->position.y = bg->world_height - j->position.h - 10;
    j->vx = 0; j->vy = 0; j->vie = 3; j->score = 0;

    bg->camera.x = 0; bg->camera.y = 0; bg->camera.w = screenWidth; bg->camera.h = screenHeight;

    printf("Level %d loaded successfully.\n", levelNum);
    return 1;
}

void mettreAJourCamera(Background *bg, const Personnage *joueur, int screenWidth) {
    if (!bg || !joueur) return;
    if (joueur->position.x < bg->camera.x + CAMERA_FOLLOW_MARGIN_X) {
        bg->camera.x = joueur->position.x - CAMERA_FOLLOW_MARGIN_X;
    } else if (joueur->position.x + joueur->position.w > bg->camera.x + screenWidth - CAMERA_FOLLOW_MARGIN_X) {
        bg->camera.x = joueur->position.x + joueur->position.w - screenWidth + CAMERA_FOLLOW_MARGIN_X;
    }
    if (bg->camera.x < 0) bg->camera.x = 0;
    if (bg->camera.x > bg->world_width - screenWidth) bg->camera.x = bg->world_width - screenWidth;
}

void afficherBackground(SDL_Surface *screen, const Background *bg) {
     if (!screen) return;
    if (bg->image) {
        SDL_Rect srcRect = bg->camera;
        SDL_Rect destRect = {0, 0, screen->w, screen->h};
        srcRect.w = screen->w; srcRect.h = screen->h;
        if(srcRect.x < 0) srcRect.x = 0; 
        if(srcRect.y < 0) srcRect.y = 0;
        if(srcRect.x + srcRect.w > bg->image->w) srcRect.w = bg->image->w - srcRect.x;
        if(srcRect.y + srcRect.h > bg->image->h) srcRect.h = bg->image->h - srcRect.y;
        SDL_BlitSurface(bg->image, &srcRect, screen, &destRect);
    } else {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 50));
    }
}
