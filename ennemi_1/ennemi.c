/*
 * ennemi.c - Implémentation des fonctions du module ennemi
 */

#include "ennemi.h"
#include <stdio.h>
#include <string.h>

/*
 * Initialise une animation avec des images individuelles
 * Paramètres:
 *   - prefixe_image: préfixe du chemin vers les images (ex: "images/ennemi_niveau1_deplacement_")
 *   - direction: direction de l'animation (0: gauche, 1: droite, 2: haut, 3: bas)
 *   - nb_frames: nombre de frames dans l'animation
 *   - delai: délai entre chaque frame
 * Retour: Pointeur vers la structure Animation initialisée
 */
Animation* initialiser_animation(const char *prefixe_image, int direction, int nb_frames, int delai) {
    Animation *anim = (Animation*) malloc(sizeof(Animation));
    if (!anim) {
        fprintf(stderr, "Erreur d'allocation mémoire pour l'animation\n");
        return NULL;
    }

    // Allocation du tableau de surfaces pour les frames individuelles
    anim->frames = (SDL_Surface**) malloc(sizeof(SDL_Surface*) * nb_frames);
    if (!anim->frames) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les frames\n");
        free(anim);
        return NULL;
    }
    
    // Chargement de chaque image individuelle
    char chemin_image[256];
    for (int i = 0; i < nb_frames; i++) {
        // Construction du chemin de l'image (ex: "images/ennemi_niveau1_deplacement_gauche_0.bmp")
        sprintf(chemin_image, "%s%s_%d.bmp", 
                prefixe_image, 
                direction == 0 ? "gauche" : 
                direction == 1 ? "droite" : 
                direction == 2 ? "haut" : "bas", 
                i);
        
        anim->frames[i] = SDL_LoadBMP(chemin_image);
        if (!anim->frames[i]) {
            fprintf(stderr, "Impossible de charger l'image %s: %s\n", chemin_image, SDL_GetError());
            
            // Libération des images déjà chargées
            for (int j = 0; j < i; j++) {
                SDL_FreeSurface(anim->frames[j]);
            }
            free(anim->frames);
            free(anim);
            return NULL;
        }
        
        // Définir la couleur transparente (fond vert)
        SDL_SetColorKey(anim->frames[i], SDL_SRCCOLORKEY, SDL_MapRGB(anim->frames[i]->format, 0, 255, 0));
    }

    anim->frame_actuelle = 0;
    anim->nb_frames = nb_frames;
    anim->delai_frame = delai;
    anim->compteur_delai = 0;
    anim->direction = direction;

    return anim;
}

/*
 * Initialise un ennemi avec ses propriétés et animations selon le niveau
 * Paramètres:
 *   - niveau: niveau du jeu (1 ou 2)
 *   - nb_entites_es: nombre d'entités ES à créer
 * Retour: Pointeur vers la structure Ennemi initialisée
 */
Ennemi* initialiser_ennemi(int niveau, int nb_entites_es) {
    Ennemi *ennemi = (Ennemi*) malloc(sizeof(Ennemi));
    if (!ennemi) {
        fprintf(stderr, "Erreur d'allocation mémoire pour l'ennemi\n");
        return NULL;
    }
    
    // Initialisation des valeurs par défaut
    ennemi->position.x = 400;
    ennemi->position.y = 300;
    ennemi->position.w = 64;
    ennemi->position.h = 64;
    ennemi->vitesse_x = 0;
    ennemi->vitesse_y = 0;
    ennemi->direction = 0;
    ennemi->etat_sante = ENNEMI_VIVANT;
    ennemi->niveau = niveau;
    ennemi->est_en_attaque = 0;
    ennemi->temps_changement_direction = 60; // En frames
    ennemi->compteur_temps = 0;
    
    // Allocation du tableau d'animations pour toutes les directions
    for (int type = 0; type < 2; type++) {
        ennemi->animations[type] = (Animation**) malloc(sizeof(Animation*) * 4); // 4 directions
        if (!ennemi->animations[type]) {
            fprintf(stderr, "Erreur d'allocation mémoire pour les animations\n");
            liberer_ennemi(ennemi);
            return NULL;
        }
    }
    
    // Initialisation des animations selon le niveau
    const char *prefixe_deplacement, *prefixe_attaque;
    int nb_frames_deplacement, nb_frames_attaque, delai_deplacement, delai_attaque;
    
    if (niveau == 1) {
        // Niveau 1: animations et vitesse plus lentes
        prefixe_deplacement = "images/ennemi_niveau1_deplacement_";
        prefixe_attaque = "images/ennemi_niveau1_attaque_";
        nb_frames_deplacement = 4;
        nb_frames_attaque = 3;
        delai_deplacement = 15;
        delai_attaque = 10;
        ennemi->vitesse_x = 2;
        ennemi->vitesse_y = 2;
    } else {
        // Niveau 2: animations et vitesse plus rapides
        prefixe_deplacement = "images/ennemi_niveau2_deplacement_";
        prefixe_attaque = "images/ennemi_niveau2_attaque_";
        nb_frames_deplacement = 6;
        nb_frames_attaque = 5;
        delai_deplacement = 10;
        delai_attaque = 8;
        ennemi->vitesse_x = 4;
        ennemi->vitesse_y = 4;
    }
    
    // Initialisation des animations pour chaque direction
    for (int dir = 0; dir < 4; dir++) {
        ennemi->animations[ANIM_DEPLACEMENT][dir] = initialiser_animation(
            prefixe_deplacement, dir, nb_frames_deplacement, delai_deplacement);
        ennemi->animations[ANIM_ATTAQUE][dir] = initialiser_animation(
            prefixe_attaque, dir, nb_frames_attaque, delai_attaque);
        
        // Vérification des erreurs
        if (!ennemi->animations[ANIM_DEPLACEMENT][dir] || !ennemi->animations[ANIM_ATTAQUE][dir]) {
            liberer_ennemi(ennemi);
            return NULL;
        }
    }
    
    // Chargement des images d'état de santé
    ennemi->images_sante[ENNEMI_VIVANT] = SDL_LoadBMP("images/ennemi_vivant.bmp");
    ennemi->images_sante[ENNEMI_BLESSE] = SDL_LoadBMP("images/ennemi_blesse.bmp");
    ennemi->images_sante[ENNEMI_NEUTRALISE] = SDL_LoadBMP("images/ennemi_neutralise.bmp");
    
    // Vérification du chargement des images
    for (int i = 0; i < 3; i++) {
        if (!ennemi->images_sante[i]) {
            fprintf(stderr, "Impossible de charger l'image d'état de santé %d: %s\n", i, SDL_GetError());
            // Nettoyage des ressources déjà allouées
            liberer_ennemi(ennemi);
            return NULL;
        }
        // Définir la couleur transparente
        SDL_SetColorKey(ennemi->images_sante[i], SDL_SRCCOLORKEY, SDL_MapRGB(ennemi->images_sante[i]->format, 0, 255, 0));
    }
    
    // Initialisation des entités ES
    ennemi->nb_entites_es = nb_entites_es;
    ennemi->entites_es = (ES*) malloc(sizeof(ES) * nb_entites_es);
    if (!ennemi->entites_es) {
        fprintf(stderr, "Erreur d'allocation mémoire pour les entités ES\n");
        liberer_ennemi(ennemi);
        return NULL;
    }
    
    // Initialisation de chaque entité ES
    for (int i = 0; i < nb_entites_es; i++) {
        ennemi->entites_es[i].image = SDL_LoadBMP("images/entite_es.bmp");
        if (!ennemi->entites_es[i].image) {
            fprintf(stderr, "Impossible de charger l'image d'entité ES: %s\n", SDL_GetError());
            liberer_ennemi(ennemi);
            return NULL;
        }
        
        SDL_SetColorKey(ennemi->entites_es[i].image, SDL_SRCCOLORKEY, SDL_MapRGB(ennemi->entites_es[i].image->format, 0, 255, 0));
        
        // Positionnement aléatoire des entités ES autour de l'ennemi
        ennemi->entites_es[i].position.w = 32;
        ennemi->entites_es[i].position.h = 32;
        ennemi->entites_es[i].position.x = ennemi->position.x + (rand() % 200) - 100;
        ennemi->entites_es[i].position.y = ennemi->position.y + (rand() % 200) - 100;
        ennemi->entites_es[i].actif = 1;
        ennemi->entites_es[i].vitesse = 1 + niveau; // Plus rapide au niveau 2
    }
    
    // Initialisation de la graine pour les nombres aléatoires
    srand(time(NULL));
    
    return ennemi;
}

/*
 * Libère toutes les ressources utilisées par l'ennemi
 * Paramètres:
 *   - ennemi: pointeur vers la structure Ennemi à libérer
 */
void liberer_ennemi(Ennemi *ennemi) {
    if (!ennemi)
        return;

    // Libération des animations
    for (int type = 0; type < 2; type++) {
        if (ennemi->animations[type]) {
            for (int dir = 0; dir < 4; dir++) {
                if (ennemi->animations[type][dir]) {
                    for (int i = 0; i < ennemi->animations[type][dir]->nb_frames; i++) {
                        if (ennemi->animations[type][dir]->frames[i]) {
                            SDL_FreeSurface(ennemi->animations[type][dir]->frames[i]);
                        }
                    }
                    free(ennemi->animations[type][dir]->frames);
                    free(ennemi->animations[type][dir]);
                }
            }
            free(ennemi->animations[type]);
        }
    }

    // Libération des images d'état de santé
    for (int i = 0; i < 3; i++) {
        if (ennemi->images_sante[i])
            SDL_FreeSurface(ennemi->images_sante[i]);
    }

    // Libération des entités ES
    if (ennemi->entites_es) {
        for (int i = 0; i < ennemi->nb_entites_es; i++) {
            if (ennemi->entites_es[i].image)
                SDL_FreeSurface(ennemi->entites_es[i].image);
        }
        free(ennemi->entites_es);
    }

    // Libération de l'ennemi lui-même
    free(ennemi);
}

/*
 * Affiche l'ennemi et ses entités ES sur l'écran
 * Paramètres:
 *   - ennemi: pointeur vers la structure Ennemi à afficher
 *   - ecran: surface SDL sur laquelle afficher
 */
void afficher_ennemi(Ennemi *ennemi, SDL_Surface *ecran) {
    if (!ennemi || !ecran)
        return;

    // Affichage des entités ES actives
    for (int i = 0; i < ennemi->nb_entites_es; i++) {
        if (ennemi->entites_es[i].actif) {
            SDL_Rect dest = ennemi->entites_es[i].position;
            SDL_BlitSurface(ennemi->entites_es[i].image, NULL, ecran, &dest);
        }
    }

    // Si l'ennemi est neutralisé, on utilise l'image correspondante
    if (ennemi->etat_sante == ENNEMI_NEUTRALISE) {
        SDL_BlitSurface(ennemi->images_sante[ENNEMI_NEUTRALISE], NULL, ecran, &ennemi->position);
    } else {
        // Sinon on utilise l'animation courante
        int type_anim = ennemi->est_en_attaque ? ANIM_ATTAQUE : ANIM_DEPLACEMENT;
        Animation *anim = ennemi->animations[type_anim][ennemi->direction];
        
        // On blit directement la frame courante (pas besoin de src rect)
        SDL_BlitSurface(anim->frames[anim->frame_actuelle], NULL, ecran, &ennemi->position);
        
        // Affichage de l'état de santé au-dessus de l'ennemi
        SDL_Rect pos_sante = ennemi->position;
        pos_sante.y -= 10;
        pos_sante.h = 5;
        SDL_BlitSurface(ennemi->images_sante[ennemi->etat_sante], NULL, ecran, &pos_sante);
    }
}

/*
 * Déplace l'ennemi de façon aléatoire dans les limites spécifiées
 * Paramètres:
 *   - ennemi: pointeur vers la structure Ennemi à déplacer
 *   - limites: rectangle définissant les limites de déplacement
 */
void deplacer_ennemi(Ennemi *ennemi, SDL_Rect limites) {
    if (!ennemi || ennemi->etat_sante == ENNEMI_NEUTRALISE)
        return;

    // Incrémenter le compteur de temps
    ennemi->compteur_temps++;
    
    // Changement de direction aléatoire après un certain temps
    if (ennemi->compteur_temps >= ennemi->temps_changement_direction) {
        ennemi->compteur_temps = 0;
        
        // Définition des trajectoires selon le niveau
        if (ennemi->niveau == 1) {
            // Niveau 1: Déplacement simple dans 4 directions
            ennemi->direction = rand() % 4;
            
            switch (ennemi->direction) {
                case 0: // Gauche
                    ennemi->vitesse_x = -2;
                    ennemi->vitesse_y = 0;
                    break;
                case 1: // Droite
                    ennemi->vitesse_x = 2;
                    ennemi->vitesse_y = 0;
                    break;
                case 2: // Haut
                    ennemi->vitesse_x = 0;
                    ennemi->vitesse_y = -2;
                    break;
                case 3: // Bas
                    ennemi->vitesse_x = 0;
                    ennemi->vitesse_y = 2;
                    break;
            }
        } else {
            // Niveau 2: Déplacement diagonal et plus rapide
            int dx = (rand() % 3) - 1; // -1, 0, 1
            int dy = (rand() % 3) - 1; // -1, 0, 1
            
            ennemi->vitesse_x = dx * 4;
            ennemi->vitesse_y = dy * 4;
            
            // Déterminer la direction principale pour l'animation
            if (abs(dx) > abs(dy)) {
                ennemi->direction = (dx > 0) ? 1 : 0; // Droite ou Gauche
            } else {
                ennemi->direction = (dy > 0) ? 3 : 2; // Bas ou Haut
            }
        }
        
        // 20% de chance de déclencher une attaque
        ennemi->est_en_attaque = (rand() % 5 == 0);
    }
    
    // Mise à jour de la position
    ennemi->position.x += ennemi->vitesse_x;
    ennemi->position.y += ennemi->vitesse_y;
    
    // Vérification des collisions avec les bords
    if (ennemi->position.x < limites.x) {
        ennemi->position.x = limites.x;
        ennemi->vitesse_x *= -1;
        ennemi->direction = 1; // Droite
    } else if (ennemi->position.x + ennemi->position.w > limites.x + limites.w) {
        ennemi->position.x = limites.x + limites.w - ennemi->position.w;
        ennemi->vitesse_x *= -1;
        ennemi->direction = 0; // Gauche
    }
    
    if (ennemi->position.y < limites.y) {
        ennemi->position.y = limites.y;
        ennemi->vitesse_y *= -1;
        ennemi->direction = 3; // Bas
    } else if (ennemi->position.y + ennemi->position.h > limites.y + limites.h) {
        ennemi->position.y = limites.y + limites.h - ennemi->position.h;
        ennemi->vitesse_y *= -1;
        ennemi->direction = 2; // Haut
    }
    
    // Déplacement des entités ES pour suivre l'ennemi
    for (int i = 0; i < ennemi->nb_entites_es; i++) {
        if (ennemi->entites_es[i].actif) {
            // Direction vers l'ennemi avec un peu d'aléatoire
            int dx = ennemi->position.x - ennemi->entites_es[i].position.x;
            int dy = ennemi->position.y - ennemi->entites_es[i].position.y;
            
            // Normalisation et application de la vitesse
            float norme = sqrt(dx * dx + dy * dy);
            if (norme > 0) {
                ennemi->entites_es[i].position.x += (dx / norme) * ennemi->entites_es[i].vitesse;
                ennemi->entites_es[i].position.y += (dy / norme) * ennemi->entites_es[i].vitesse;
            }
            
            // Ajout d'un peu d'aléatoire dans le mouvement
            if (rand() % 20 == 0) {
                ennemi->entites_es[i].position.x += (rand() % 7) - 3;
                ennemi->entites_es[i].position.y += (rand() % 7) - 3;
            }
            
            // Vérification des limites pour les entités ES
            if (ennemi->entites_es[i].position.x < limites.x)
                ennemi->entites_es[i].position.x = limites.x;
            else if (ennemi->entites_es[i].position.x + ennemi->entites_es[i].position.w > limites.x + limites.w)
                ennemi->entites_es[i].position.x = limites.x + limites.w - ennemi->entites_es[i].position.w;
                
            if (ennemi->entites_es[i].position.y < limites.y)
                ennemi->entites_es[i].position.y = limites.y;
            else if (ennemi->entites_es[i].position.y + ennemi->entites_es[i].position.h > limites.y + limites.h)
                ennemi->entites_es[i].position.y = limites.y + limites.h - ennemi->entites_es[i].position.h;
        }
    }
}

/*
 * Anime l'ennemi en mettant à jour les frames d'animation
 * Paramètres:
 *   - ennemi: pointeur vers la structure Ennemi à animer
 */
void animer_ennemi(Ennemi *ennemi) {
    if (!ennemi || ennemi->etat_sante == ENNEMI_NEUTRALISE)
        return;

    // Récupération de l'animation en cours
    int type_anim = ennemi->est_en_attaque ? ANIM_ATTAQUE : ANIM_DEPLACEMENT;
    Animation *anim = ennemi->animations[type_anim][ennemi->direction];
    
    // Incrémentation du compteur de délai
    anim->compteur_delai++;
    
    // Passage à la frame suivante si le délai est atteint
    if (anim->compteur_delai >= anim->delai_frame) {
        anim->compteur_delai = 0;
        anim->frame_actuelle = (anim->frame_actuelle + 1) % anim->nb_frames;
        
        // Si l'animation d'attaque est terminée, revenir à l'animation de déplacement
        if (ennemi->est_en_attaque && anim->frame_actuelle == 0) {
            ennemi->est_en_attaque = 0;
        }
    }
}

/*
 * Détecte une collision entre le joueur et l'ennemi
 * Paramètres:
 *   - pos_joueur: rectangle représentant la position du joueur
 *   - ennemi: pointeur vers la structure Ennemi
 * Retour: 1 si collision, 0 sinon
 */
int detecter_collision_joueur_ennemi(SDL_Rect pos_joueur, Ennemi *ennemi) {
    if (!ennemi || ennemi->etat_sante == ENNEMI_NEUTRALISE)
        return 0;

    // Vérification de la collision entre les rectangles
    if (pos_joueur.x < ennemi->position.x + ennemi->position.w &&
        pos_joueur.x + pos_joueur.w > ennemi->position.x &&
        pos_joueur.y < ennemi->position.y + ennemi->position.h &&
        pos_joueur.y + pos_joueur.h > ennemi->position.y) {
        return 1;
    }
    
    return 0;
}

/*
 * Détecte une collision entre le joueur et les entités ES
 * Paramètres:
 *   - pos_joueur: rectangle représentant la position du joueur
 *   - ennemi: pointeur vers la structure Ennemi contenant les entités ES
 * Retour: Indice de l'entité ES en collision, -1 si aucune collision
 */
int detecter_collision_joueur_es(SDL_Rect pos_joueur, Ennemi *ennemi) {
    if (!ennemi)
        return -1;

    for (int i = 0; i < ennemi->nb_entites_es; i++) {
        if (ennemi->entites_es[i].actif &&
            pos_joueur.x < ennemi->entites_es[i].position.x + ennemi->entites_es[i].position.w &&
            pos_joueur.x + pos_joueur.w > ennemi->entites_es[i].position.x &&
            pos_joueur.y < ennemi->entites_es[i].position.y + ennemi->entites_es[i].position.h &&
            pos_joueur.y + pos_joueur.h > ennemi->entites_es[i].position.y) {
            return i;
        }
    }
    
    return -1;
}

/*
 * Gère l'état de santé de l'ennemi en fonction des dommages reçus
 * Paramètres:
 *   - ennemi: pointeur vers la structure Ennemi
 *   - dommage: quantité de dommage infligée (1 pour blessé, 2 pour neutralisé)
 */
void gerer_sante_ennemi(Ennemi *ennemi, int dommage) {
    if (!ennemi || ennemi->etat_sante == ENNEMI_NEUTRALISE)
        return;

    // Application des dommages
    ennemi->etat_sante += dommage;
    
    // Limitation de l'état de santé
    if (ennemi->etat_sante > ENNEMI_NEUTRALISE)
        ennemi->etat_sante = ENNEMI_NEUTRALISE;
    
    // Si l'ennemi est neutralisé, désactiver ses entités ES
    if (ennemi->etat_sante == ENNEMI_NEUTRALISE) {
        for (int i = 0; i < ennemi->nb_entites_es; i++) {
            ennemi->entites_es[i].actif = 0;
        }
    }
}
