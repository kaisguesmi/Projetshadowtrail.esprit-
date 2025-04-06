#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "perso.h"

void initialiserperso(personne *p)
{
    int i, j;
    char nom_image[20];
    p->acceleration = 0;
    p->direction = 0;
    p->nbr_frame = 0;
    p->position.x = 10;
    p->position.y = 500;
    p->posrel.x = -50;
    p->posrel.y = 0;
    p->vitesse = 0;
    p->vitesse_saut = 0;
    p->up = 0;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 11; j++)
        {   
            sprintf(nom_image, "animation/%d/%d.png", i, j);
            p->images[i][j] = IMG_Load(nom_image);
        }
    }
}

void initialiserperso2(personne *p)
{
    int i, j;
    char nom_image[20];
    p->acceleration = 0;
    p->direction = 0;
    p->nbr_frame = 0;
    p->position.x = 300;
    p->position.y = 500;
    p->posrel.x = -50;
    p->posrel.y = 0;
    p->vitesse = 0;
    p->vitesse_saut = 0;
    p->up = 0;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 11; j++)
        {   

            sprintf(nom_image, "animation/%d/%d.png", i, j + 11);
            p->images[i][j] = IMG_Load(nom_image);
        }
    }
}

void afficherperso(personne p, SDL_Surface *screen)
{
    SDL_BlitSurface(p.images[p.direction][p.nbr_frame], NULL, screen, &p.position);
}

void moveperso(personne *p, int dt)
{
    double dx;
    dx = 0.5 * p->acceleration * dt * dt + p->vitesse * dt;	
    if ((p->direction == 0) && (p->position.x <= 900))
    {
        p->position.x = p->position.x + dx;        
    }
    else if ((p->direction == 1) && (p->position.x >= 20))
    {
        p->position.x = p->position.x - dx;
    }
}

void animerperso(personne* p)
{
    p->nbr_frame++;
    if (p->nbr_frame >= 11)
    {
        p->nbr_frame = 0;
    }
    if (p->vitesse == 0)
    {
        p->nbr_frame = 0;
    }
}

void saut(personne* p, int posabsx, int posabsy)
{
    if (p->up == 1)
    {
        if (p->position.y == 500)
        {
            p->vitesse_saut = -50;
        }
    }
    else if (p->up == 2)
    {
        if (p->direction == 0)
        {
            if (p->posrel.x >= 50)
            {
                p->posrel.x = -50;
                p->up = 0;
            }
            else
            {
                p->posrel.x = p->posrel.x + 2;
            }
            
            p->posrel.y = (-0.06 * (p->posrel.x * p->posrel.x) + 150);
            p->position.x = posabsx + p->posrel.x + 50;
            p->position.y = posabsy - p->posrel.y; 
            if (p->posrel.x == -50)
                p->position.x = p->position.x + 100;
        }       
        else if (p->direction == 1)
        {
            if (p->posrel.x <= -50)
            {
                p->posrel.x = 50;
                p->up = 0;
            }
            else
            {
                p->posrel.x = p->posrel.x - 2;
            }
            
            p->posrel.y = (-0.06 * (p->posrel.x * p->posrel.x) + 150);
            p->position.x = posabsx + p->posrel.x - 50;
            p->position.y = posabsy - p->posrel.y; 
            if (p->posrel.x == 50)
                p->position.x = p->position.x - 100;
        }       
    }
}

