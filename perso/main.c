#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "perso.h"

int main()
{    
    int collision = 0;
    personne p, p1;
    SDL_Surface *screen;
    SDL_Surface *back;
    int continuer = 1;
    SDL_Event event; 
    int acceleration = -1;
    int deceleration = -1;
    Uint32 t_prev, dt;
    int posabsx, posabsy;
    int acceleration1 = -1;
    int deceleration1 = -1;
    int posabsx1, posabsy1;

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(1200, 720, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    back = IMG_Load("background.png");
    initialiserperso(&p);
    initialiserperso2(&p1);
    posabsx = p.position.x;
    posabsy = p.position.y;
    posabsx1 = p1.position.x;
    posabsy1 = p1.position.y;

    while (continuer)
    {
        t_prev = SDL_GetTicks();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    continuer = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RIGHT:
                            p.direction = 0;
                            p.vitesse = 5;
                            acceleration = 1;
                            deceleration = 0;
                            break;
                        case SDLK_LEFT:
                            p.direction = 1;
                            p.vitesse = 5;	
                            acceleration = 1;
                            deceleration = 0;
                            break;
                        case SDLK_UP:
                            p.up = 1;
                            break;
                        case SDLK_DOWN:
                            p.up = 2;
                            break;
                        case SDLK_d:
                            p1.direction = 0;
                            p1.vitesse = 5;
                            acceleration1 = 1;
                            deceleration1 = 0;
                            break;
                        case SDLK_q:
                            p1.direction = 1;
                            p1.vitesse = 5;	
                            acceleration1 = 1;
                            deceleration1 = 0;
                            break;
                        case SDLK_z:
                            p1.up = 1;
                            break;
                        case SDLK_s:
                            p1.up = 2;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_RIGHT:
                            p.vitesse = 0;
                            acceleration = 0;
                            deceleration = 1;
                            break;
                        case SDLK_LEFT:
                            p.vitesse = 0;
                            acceleration = 0;
                            deceleration = 1;
                            break;
                        case SDLK_UP:
                            p.up = 0;
                            break;
                        case SDLK_d:
                            p1.vitesse = 0;
                            acceleration1 = 0;
                            deceleration1 = 1;
                            break;
                        case SDLK_q:
                            p1.vitesse = 0;
                            acceleration1 = 0;
                            deceleration1 = 1;
                            break;
                        case SDLK_z:
                            p1.up = 0;
                            break;
                    }
                    break;
            }
        }

        if (acceleration == 1)
            p.acceleration += 0.05;
        if (deceleration == 1)
            p.acceleration -= 0.05;
        p.acceleration -= 0.009;
        if (p.acceleration < 0)
            p.acceleration = 0;
        SDL_Delay(3);
        dt = SDL_GetTicks() - t_prev;
        moveperso(&p, dt);
        animerperso(&p);
        
        if (p.up == 1 || p.up == 0)
        {	
            saut(&p, posabsx, posabsy);
            p.position.y = p.position.y + p.vitesse_saut;
            p.vitesse_saut = p.vitesse_saut + 10;
        }
        if (p.position.y > 300)
        {   
            p.position.y = 300;
            p.vitesse_saut = 0;
            p.up = 0;
        }
        
        if (p.up == 0)
        {
            posabsx = p.position.x;
            posabsy = p.position.y;
        }
        if (p.up == 2)
        {
            saut(&p, posabsx, posabsy);
            if (p.position.y > 300 && p.direction == 0)
            {
                p.position.y = 300;
                p.position.x = p.position.x + p.posrel.x + 150;
                p.up = 0;
            }
            if (p.position.y > 300 && p.direction == 1)
            {
                p.position.y = 300;
                p.position.x = p.position.x + p.posrel.x - 150;
                p.up = 0;
            }
        }

        if (acceleration1 == 1)
            p1.acceleration += 0.05;
        if (deceleration1 == 1)
            p1.acceleration -= 0.05;
        p1.acceleration -= 0.009;
        if (p1.acceleration < 0)
            p1.acceleration = 0;
        SDL_Delay(3);

        moveperso(&p1, dt);
        animerperso(&p1);
        if (p1.up == 1 || p1.up == 0)
        {	
            saut(&p1, posabsx1, posabsy1);
            p1.position.y = p1.position.y + p1.vitesse_saut;
            p1.vitesse_saut = p1.vitesse_saut + 10;
        }
        if (p1.position.y > 300)
        {   
            p1.position.y = 300;
            p1.vitesse_saut = 0;
            p1.up = 0;
        }
        
        if (p1.up == 0)
        {
            posabsx1 = p1.position.x;
            posabsy1 = p1.position.y;
        }
        if (p1.up == 2)
        {
            saut(&p1, posabsx1, posabsy1);
            if (p1.position.y > 300 && p1.direction == 0)
            {
                p1.position.y = 300;
                p1.position.x = p1.position.x + p1.posrel.x + 150;
                p1.up = 0;
            }
            if (p1.position.y > 300 && p1.direction == 1)
            {
                p1.position.y = 300;
                p1.position.x = p1.position.x + p1.posrel.x - 150;
                p1.up = 0;
            }
        }

        SDL_BlitSurface(back, NULL, screen, NULL);
        afficherperso(p, screen);
        afficherperso(p1, screen);
        if (p.position.x == 200)
            collision = 1;
	    
        SDL_Flip(screen);
        SDL_Delay(30);
    }
		
    SDL_Quit();
    return 0;
}

