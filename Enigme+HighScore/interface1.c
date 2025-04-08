#include "interface1.h"
#include <stdio.h>
#include <string.h>

void initInterface1(Background *background, Button *buttons, Mix_Chunk **sound, Texte *label, Texte *inputText) {
    background->T[0] = IMG_Load("bg.jpg");
    if (!background->T[0]) {
        printf("Erreur : Impossible de charger bg.jpg.\n");
        return;
    }
    background->music = NULL;
    
    buttons[0].TB[0] = IMG_Load("bt1.jpg");
    buttons[0].TB[1] = IMG_Load("bt2.jpg");
    if (!buttons[0].TB[0] || !buttons[0].TB[1]) {
        printf("Erreur : Impossible de charger les images du bouton.\n");
        return;
    }
    buttons[0].etat = 0;
    buttons[0].pos_img_ecran.x = 1470;
    buttons[0].pos_img_ecran.y = 800;
    
    *sound = Mix_LoadWAV("simple.wav");
    if (!*sound) {
        printf("Erreur : Impossible de charger simple.wav.\n");
        return;
    }
    
    label->police = TTF_OpenFont("Joe Caps Underwood.ttf", 40);
    if (!label->police) {
        printf("Erreur : Impossible de charger la police Joe Caps Underwood.ttf.\n");
        return;
    }
    label->color_txt.r = 0;
    label->color_txt.g = 0;
    label->color_txt.b = 0;
    label->pos_txt.x = 790;
    label->pos_txt.y = 320;
    
    inputText->police = TTF_OpenFont("Joe Caps Underwood.ttf", 40);
    if (!inputText->police) {
        printf("Erreur : Impossible de charger la police Joe Caps Underwood.ttf.\n");
        return;
    }
    inputText->color_txt.r = 0;
    inputText->color_txt.g = 0;
    inputText->color_txt.b = 0;
    inputText->pos_txt.x = 790;
    inputText->pos_txt.y = 410;
    strcpy(inputText->name, "");
    inputText->textSurface = NULL;
}

void displayInterface1(SDL_Surface *screen, Background *background, Button *buttons, Texte *label, Texte *inputText) {
    SDL_BlitSurface(background->T[0], NULL, screen, NULL);
    
    label->txt = TTF_RenderText_Solid(label->police, "Saisir Pseudo :", label->color_txt);
    if (!label->txt) {
        printf("Erreur : Impossible de rendre le texte du label.\n");
        return;
    }
    SDL_BlitSurface(label->txt, NULL, screen, &label->pos_txt);
    SDL_FreeSurface(label->txt);
    
    if (strlen(inputText->name) > 0) {
        if (inputText->textSurface)
            SDL_FreeSurface(inputText->textSurface);
        inputText->textSurface = TTF_RenderText_Solid(inputText->police, inputText->name, inputText->color_txt);
        if (inputText->textSurface)
            SDL_BlitSurface(inputText->textSurface, NULL, screen, &inputText->pos_txt);
    } else {
        if (inputText->textSurface)
            SDL_FreeSurface(inputText->textSurface);
        inputText->textSurface = TTF_RenderText_Solid(inputText->police, "_ _ _ _ _ _", inputText->color_txt);
        if (inputText->textSurface)
            SDL_BlitSurface(inputText->textSurface, NULL, screen, &inputText->pos_txt);
    }
    
    if (buttons[0].TB[buttons[0].etat])
        SDL_BlitSurface(buttons[0].TB[buttons[0].etat], NULL, screen, &buttons[0].pos_img_ecran);
    
    SDL_Flip(screen);
}

int handleInterface1Events(Button *buttons, int numButtons, Mix_Chunk *sound, Texte *inputText) {
    (void)numButtons;  // Marquer le paramètre comme inutilisé pour éviter le warning

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            return 0;
        
        // Gestion du clavier
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                return 1;
            } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                if (strlen(inputText->name) > 0)
                    inputText->name[strlen(inputText->name) - 1] = '\0';
            } else {
                Uint16 unicode = event.key.keysym.unicode;
                if (unicode >= 32 && unicode <= 126) {
                    int len = strlen(inputText->name);
                    if (len < MAX_NAME_LENGTH - 1) {
                        inputText->name[len] = (char)unicode;
                        inputText->name[len + 1] = '\0';
                    }
                }
            }
        }
        
        // Gestion de la souris
        if (event.type == SDL_MOUSEMOTION) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (buttons[0].TB[0]) {
                if (x >= buttons[0].pos_img_ecran.x && x <= buttons[0].pos_img_ecran.x + buttons[0].TB[0]->w &&
                    y >= buttons[0].pos_img_ecran.y && y <= buttons[0].pos_img_ecran.y + buttons[0].TB[0]->h) {
                    if (buttons[0].etat == 0) {
                        buttons[0].etat = 1;
                        if (sound)
                            Mix_PlayChannel(-1, sound, 0);
                    }
                } else {
                    if (buttons[0].etat == 1)
                        buttons[0].etat = 0;
                }
            }
        }
        
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (buttons[0].TB[0] &&
                x >= buttons[0].pos_img_ecran.x && x <= buttons[0].pos_img_ecran.x + buttons[0].TB[0]->w &&
                y >= buttons[0].pos_img_ecran.y && y <= buttons[0].pos_img_ecran.y + buttons[0].TB[0]->h) {
                return 1;
            }
        }
    }
    return -1;
}

void freeInterface1(Background *background, Button *buttons, Mix_Chunk *sound, Texte *label, Texte *inputText) {
    if (background->T[0]) SDL_FreeSurface(background->T[0]);
    if (buttons[0].TB[0]) SDL_FreeSurface(buttons[0].TB[0]);
    if (buttons[0].TB[1]) SDL_FreeSurface(buttons[0].TB[1]);
    if (sound) Mix_FreeChunk(sound);
    if (label->police) TTF_CloseFont(label->police);
    if (inputText->textSurface) SDL_FreeSurface(inputText->textSurface);
    if (inputText->police) TTF_CloseFont(inputText->police);
}

int runInterface1(SDL_Surface *screen) {
    Background bg;
    Button button[1];
    Mix_Chunk *sound = NULL;
    Texte label, inputText;
    
    initInterface1(&bg, button, &sound, &label, &inputText);
    
    int running = 1;
    int ret = -1;
    while (running) {
        displayInterface1(screen, &bg, button, &label, &inputText);
        ret = handleInterface1Events(button, 1, sound, &inputText);
        if (ret == 1) { 
            running = 0;
        } else if (ret == 0) { 
            running = 0;
            return 0;
        }
    }
    
    freeInterface1(&bg, button, sound, &label, &inputText);
    return ret;
}

