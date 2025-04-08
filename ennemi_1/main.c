/*
 * main.c - Exemple d'utilisation du module ennemi
 */

#include <SDL/SDL.h>
#include <stdio.h>
#include "ennemi.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP    32

int main(int argc, char *argv[]) {
    SDL_Surface *ecran = NULL;
    SDL_Event event;
    int continuer = 1;
    
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    // Création de la fenêtre
    ecran = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!ecran) {
        fprintf(stderr, "Impossible de créer la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    SDL_WM_SetCaption("Module Ennemi - Démo", NULL);
    
    // Définition des limites du jeu (pour le déplacement)
    SDL_Rect limites_jeu = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    
    // Initialisation du joueur (pour les tests de collision)
    SDL_Rect pos_joueur = {100, 100, 64, 64};
    SDL_Surface *image_joueur = SDL_LoadBMP("images/joueur.bmp");
    if (!image_joueur) {
        fprintf(stderr, "Impossible de charger l'image du joueur: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_SetColorKey(image_joueur, SDL_SRCCOLORKEY, SDL_MapRGB(image_joueur->format, 0, 255, 0));
    
    // Initialisation de l'ennemi (niveau 1, 3 entités ES)
    Ennemi *ennemi = initialiser_ennemi(1, 3);
    if (!ennemi) {
        fprintf(stderr, "Erreur lors de l'initialisation de l'ennemi\n");
        SDL_FreeSurface(image_joueur);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    // Boucle principale
    while (continuer) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    continuer = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            continuer = 0;
                            break;
                        case SDLK_UP:
                            pos_joueur.y -= 5;
                            break;
                        case SDLK_DOWN:
                            pos_joueur.y += 5;
                            break;
                        case SDLK_LEFT:
                            pos_joueur.x -= 5;
                            break;
                        case SDLK_RIGHT:
                            pos_joueur.x += 5;
                            break;
                        case SDLK_SPACE:
                            // Blesser l'ennemi quand on appuie sur espace
                            gerer_sante_ennemi(ennemi, 1);
                            break;
			 default: 
    			     break;                           
                    }
                    break;
            }
        }
        
        // Vérification des limites pour le joueur
        if (pos_joueur.x < 0) pos_joueur.x = 0;
        if (pos_joueur.y < 0) pos_joueur.y = 0;
        if (pos_joueur.x > SCREEN_WIDTH - pos_joueur.w) pos_joueur.x = SCREEN_WIDTH - pos_joueur.w;
        if (pos_joueur.y > SCREEN_HEIGHT - pos_joueur.h) pos_joueur.y = SCREEN_HEIGHT - pos_joueur.h;
        
        // Détection des collisions
        if (detecter_collision_joueur_ennemi(pos_joueur, ennemi)) {
            printf("Collision avec l'ennemi!\n");
            // Si l'ennemi est en attaque, on peut blesser le joueur ici
        }
        
        int collision_es = detecter_collision_joueur_es(pos_joueur, ennemi);
        if (collision_es != -1) {
            printf("Collision avec l'entité ES %d!\n", collision_es);
            // On peut désactiver l'entité ES si nécessaire
            ennemi->entites_es[collision_es].actif = 0;
        }
        
        // Déplacement et animation de l'ennemi
        deplacer_ennemi(ennemi, limites_jeu);
        animer_ennemi(ennemi);
        
        // Effacement de l'écran
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
        
        // Affichage du joueur
        SDL_BlitSurface(image_joueur, NULL, ecran, &pos_joueur);
        
        // Affichage de l'ennemi et de ses entités
        afficher_ennemi(ennemi, ecran);
        
        // Mise à jour de l'écran
        SDL_Flip(ecran);
        
        // Ajout d'un délai pour limiter la vitesse
        SDL_Delay(16);  // ~60 FPS
    }
    
    // Libération des ressources
    liberer_ennemi(ennemi);
    SDL_FreeSurface(image_joueur);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
