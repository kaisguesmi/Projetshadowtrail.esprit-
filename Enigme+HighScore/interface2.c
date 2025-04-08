#include "interface2.h"
#include <stdio.h>
#include <string.h>

void initEtoile(Etoile *e, const char *imagePath, int x, int y) {
    e->image = IMG_Load(imagePath);
    if (!e->image) {
        printf("Erreur : Impossible de charger l'image de l'étoile (%s).\n", imagePath);
        return;
    }
    e->position.x = x;
    e->position.y = y;
}

void displayEtoile(SDL_Surface *screen, Etoile *e) {
    if (e->image && screen) {
        SDL_BlitSurface(e->image, NULL, screen, &e->position);
    }
}

void freeEtoile(Etoile *e) {
    if (e->image) {
        SDL_FreeSurface(e->image);
        e->image = NULL;
    }
}

void initInterface2(Background2 *background, Button2 *buttons, Mix_Chunk **sound, Texte2 *label, Texte2 *texte2, Texte2 *texte3, Etoile *etoile1, Etoile *etoile2, Etoile *etoile3) {
    background->T[0] = IMG_Load("bg.jpg");
    if (!background->T[0]) {
        printf("Erreur : Impossible de charger bg.jpg.\n");
        return;
    }
    
    initEtoile(etoile1, "star.png", 600, 260);
    initEtoile(etoile2, "star.png", 600, 500);
    initEtoile(etoile3, "star.png", 600, 690);
    
    background->music = Mix_LoadMUS("Audiorezout - Heroic Victory.mp3");
    if (!background->music) {
        printf("Erreur : Impossible de charger la musique de fond.\n");
    } else {
        Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
        Mix_PlayMusic(background->music, -1);
    }
    
    buttons[0].TB[0] = IMG_Load("button2.png");
    buttons[0].TB[1] = IMG_Load("button1.png");
    buttons[0].etat = 0;
    buttons[0].pos_img_ecran.x = 1130;
    buttons[0].pos_img_ecran.y = 880;
    
    buttons[1].TB[0] = IMG_Load("back.jpg");
    buttons[1].TB[1] = IMG_Load("back0.jpg");
    buttons[1].etat = 0;
    buttons[1].pos_img_ecran.x = 1470;
    buttons[1].pos_img_ecran.y = 880;
    
    *sound = Mix_LoadWAV("simple.wav");
    if (!*sound) {
        printf("Erreur : Impossible de charger simple.wav.\n");
        return;
    }
    
    label->police = TTF_OpenFont("Joe Caps Underwood.ttf", 40);
    texte2->police = TTF_OpenFont("Joe Caps Underwood.ttf", 40);
    texte3->police = TTF_OpenFont("Joe Caps Underwood.ttf", 40);
    if (!label->police || !texte2->police || !texte3->police) {
        printf("Erreur : Impossible de charger la police.\n");
        return;
    }
    label->color_txt.r = 0;
    label->color_txt.g = 0;
    label->color_txt.b = 0;
    texte2->color_txt.r = 0;
    texte2->color_txt.g = 0;
    texte2->color_txt.b = 0;
    texte3->color_txt.r = 0;
    texte3->color_txt.g = 0;
    texte3->color_txt.b = 0;
    
    label->pos_txt.x = 800;
    label->pos_txt.y = 340;
    texte2->pos_txt.x = 800;
    texte2->pos_txt.y = 580;
    texte3->pos_txt.x = 800;
    texte3->pos_txt.y = 775;
}

void displayInterface2(SDL_Surface *screen, Background2 *background, Button2 *buttons, Texte2 *label, Texte2 *texte2, Texte2 *texte3, Etoile *etoile1, Etoile *etoile2, Etoile *etoile3) {
    SDL_BlitSurface(background->T[0], NULL, screen, NULL);
    displayEtoile(screen, etoile1);
    displayEtoile(screen, etoile2);
    displayEtoile(screen, etoile3);
    
    label->txt = TTF_RenderText_Solid(label->police, "MissTagada", label->color_txt);
    texte2->txt = TTF_RenderText_Solid(texte2->police, "Imprina", texte2->color_txt);
    texte3->txt = TTF_RenderText_Solid(texte3->police, "Morningstar", texte3->color_txt);
    
    SDL_BlitSurface(label->txt, NULL, screen, &label->pos_txt);
    SDL_BlitSurface(texte2->txt, NULL, screen, &texte2->pos_txt);
    SDL_BlitSurface(texte3->txt, NULL, screen, &texte3->pos_txt);
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        SDL_BlitSurface(buttons[i].TB[buttons[i].etat], NULL, screen, &buttons[i].pos_img_ecran);
    }
    SDL_Flip(screen);
}

int handleInterface2Events(Button2 *buttons, int numButtons, Mix_Chunk *sound) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return 0;
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_e:
                    return 2; // Exemple : passer à un autre sous-menu (si besoin)
                case SDLK_ESCAPE:
                    return 0;
                default:
                    break;
            }
        }
        if (event.type == SDL_MOUSEMOTION) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            for (int i = 0; i < numButtons; i++) {
                if (buttons[i].TB[0]) {
                    if (x >= buttons[i].pos_img_ecran.x && x <= buttons[i].pos_img_ecran.x + buttons[i].TB[0]->w &&
                        y >= buttons[i].pos_img_ecran.y && y <= buttons[i].pos_img_ecran.y + buttons[i].TB[0]->h) {
                        if (buttons[i].etat == 0) {
                            buttons[i].etat = 1;
                            Mix_PlayChannel(-1, sound, 0);
                        }
                    } else {
                        if (buttons[i].etat == 1)
                            buttons[i].etat = 0;
                    }
                }
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            for (int i = 0; i < numButtons; i++) {
                if (buttons[i].TB[0] &&
                    x >= buttons[i].pos_img_ecran.x && x <= buttons[i].pos_img_ecran.x + buttons[i].TB[0]->w &&
                    y >= buttons[i].pos_img_ecran.y && y <= buttons[i].pos_img_ecran.y + buttons[i].TB[0]->h) {
                    printf("Bouton %d cliqué !\n", i+1);
                    if (i == 0)
                        return 0; // Quitter l’interface 2
                    else if (i == 1)
                        return 1; // Retour (par exemple, au menu principal)
                }
            }
        }
    }
    return -1;
}

void freeInterface2(Background2 *background, Button2 *buttons, Mix_Chunk *sound, Texte2 *label, Texte2 *texte2, Texte2 *texte3, Etoile *etoile1, Etoile *etoile2, Etoile *etoile3) {
    if (background->T[0]) SDL_FreeSurface(background->T[0]);
    if (background->music) {
        Mix_HaltMusic();
        Mix_FreeMusic(background->music);
    }
    freeEtoile(etoile1);
    freeEtoile(etoile2);
    freeEtoile(etoile3);
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].TB[0]) SDL_FreeSurface(buttons[i].TB[0]);
        if (buttons[i].TB[1]) SDL_FreeSurface(buttons[i].TB[1]);
    }
    if (sound) {
        Mix_HaltChannel(-1);
        Mix_FreeChunk(sound);
    }
    if (label->police) TTF_CloseFont(label->police);
    if (texte2->police) TTF_CloseFont(texte2->police);
    if (texte3->police) TTF_CloseFont(texte3->police);
}

int runInterface2(SDL_Surface *screen) {
    Background2 bg;
    Button2 buttons[NUM_BUTTONS];
    Mix_Chunk *sound = NULL;
    Texte2 label, texte2, texte3;
    Etoile etoile1, etoile2, etoile3;
    
    initInterface2(&bg, buttons, &sound, &label, &texte2, &texte3, &etoile1, &etoile2, &etoile3);
    
    int running = 1;
    int ret = -1;
    while (running) {
        displayInterface2(screen, &bg, buttons, &label, &texte2, &texte3, &etoile1, &etoile2, &etoile3);
        ret = handleInterface2Events(buttons, NUM_BUTTONS, sound);
        if (ret == 0 || ret == 1 || ret == 2)
            running = 0;
    }
    
    freeInterface2(&bg, buttons, sound, &label, &texte2, &texte3, &etoile1, &etoile2, &etoile3);
    return ret;
}

