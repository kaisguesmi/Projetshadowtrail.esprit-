#ifndef ENIGME_H
#define ENIGME_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#define MAX_QUESTIONS 100

typedef struct {
    SDL_Surface *txt;
    SDL_Rect pos_txt;
    SDL_Color color_txt;
    TTF_Font *police;
} texte;

typedef struct {
    SDL_Surface *T[4];
    SDL_Rect pos_img_affiche;
    Mix_Music *music;
    int niv;
} bg;

typedef struct {
    SDL_Surface *TB[2];
    char url[20];
    char url2[20];
    SDL_Rect pos_img_ecran;
    int etat;
} bouton;

typedef struct {
    SDL_Surface *song;
    char url[20];
    SDL_Rect pos_img_ecran;
} image;

typedef struct {
    char question[256];
    char answers[3][256];
    int correct_answer;
} quiz_data;

typedef struct {
    SDL_Surface *question;
    SDL_Rect pos_question;
    SDL_Surface *answer_btn[3][2];
    SDL_Surface *answer_text[3];
    SDL_Rect pos_answer_btn[3];
    SDL_Rect pos_answer_text[3];
    int active;
    int current_question;
    quiz_data questions[MAX_QUESTIONS];
    int total_questions;
    int answer_hover_state[3];
} quiz_state;

// Fonctions existantes
int load_questions(const char *filename, quiz_data *questions);
void shuffle_questions(quiz_data *questions, int count);
void init_enigma(bg *background, bouton *quiz_btn, bouton *puzzle_btn, SDL_Surface **screen, quiz_state *quiz);
void display_enigma(SDL_Surface *screen, bg background, bouton quiz_btn, bouton puzzle_btn, quiz_state quiz);
void handle_enigma_events(SDL_Event event, int *running, int *quiz_selected, int *puzzle_selected, bouton *quiz_btn, bouton *puzzle_btn, quiz_state *quiz, bg *background);
void free_enigma(bg *background, bouton *quiz_btn, bouton *puzzle_btn, quiz_state *quiz);

// Fonction d'exécution de l'interface 3 (énigme) pour l'intégration
int runEnigme(SDL_Surface *screen);

#endif // ENIGME_H

