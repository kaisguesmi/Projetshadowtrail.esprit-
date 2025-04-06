#ifndef PERSONE_H
#define PERSONE_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

typedef struct
{
    SDL_Surface *images[2][22];
    SDL_Rect position, posrel;
    int direction;
    int nbr_frame;
    double vitesse, acceleration, vitesse_saut;
    int up;
} personne;

void initialiserperso(personne* p);
void initialiserperso2(personne* p);
void afficherperso(personne p, SDL_Surface *screen);
void moveperso(personne *p, int dt);
void animerperso(personne* p);
void saut(personne* p, int posabsx, int posabsy);

#endif

