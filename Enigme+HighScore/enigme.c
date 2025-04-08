#include "enigme.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SCREEN_H 1080
#define SCREEN_W 1920
#define MAX_LINE_LENGTH 1024
static Mix_Chunk *hover_sound = NULL;

int load_questions(const char *filename, quiz_data *questions) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open questions file!\n");
        return 0;
    }
    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (count >= MAX_QUESTIONS) break;
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        char *token = strtok(line, "|");
        if (token) {
            strncpy(questions[count].question, token, sizeof(questions[count].question) - 1);
            for (int i = 0; i < 3; i++) {
                token = strtok(NULL, "|");
                if (token) {
                    int len = strlen(token);
                    if (token[len - 1] == '*') {
                        token[len - 1] = '\0';
                        questions[count].correct_answer = i;
                    }
                    strncpy(questions[count].answers[i], token, sizeof(questions[count].answers[i]) - 1);
                } else {
                    strncpy(questions[count].answers[i], "", sizeof(questions[count].answers[i]) - 1);
                }
            }
            count++;
        }
    }
    fclose(file);
    return count;
}

void shuffle_questions(quiz_data *questions, int count) {
    srand(time(NULL));
    for (int i = 0; i < count - 1; i++) {
        int j = i + rand() % (count - i);
        quiz_data temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

void init_enigma(bg *background, bouton *quiz_btn, bouton *puzzle_btn, SDL_Surface **screen, quiz_state *quiz) {
    (void)screen;  // Évite l'avertissement pour le paramètre inutilisé
    background->music = Mix_LoadMUS("quiz.mp3");
    if (!background->music) {
        printf("Failed to load music! Mix_Error: %s\n", Mix_GetError());
    }
    background->T[0] = IMG_Load("background4.png");
    background->pos_img_affiche.x = 0;
    background->pos_img_affiche.y = 0;
    
    quiz_btn->TB[0] = IMG_Load("quiz_normal.png");
    quiz_btn->TB[1] = IMG_Load("quiz_hover.png");
    quiz_btn->pos_img_ecran.x = 1050;
    quiz_btn->pos_img_ecran.y = 500;
    quiz_btn->etat = 0;
    
    puzzle_btn->TB[0] = IMG_Load("puzzle_normal.png");
    puzzle_btn->TB[1] = IMG_Load("puzzle_hover.png");
    puzzle_btn->pos_img_ecran.x = 1050;
    puzzle_btn->pos_img_ecran.y = 750;
    puzzle_btn->etat = 0;
    
    quiz->total_questions = load_questions("questions.txt", quiz->questions);
    if (quiz->total_questions == 0) {
        printf("No questions loaded!\n");
        return;
    }
    shuffle_questions(quiz->questions, quiz->total_questions);
    quiz->current_question = 0;
    quiz->active = 0;
    for (int i = 0; i < 3; i++) {
        quiz->answer_hover_state[i] = 0;
    }
    SDL_Color textColor = {255, 255, 255, 0};
    TTF_Font *font = TTF_OpenFont("Joe Caps Underwood.ttf", 19);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
    }
    hover_sound = Mix_LoadWAV("simple.wav");
    if (!hover_sound) {
        printf("Failed to load hover sound! Mix_Error: %s\n", Mix_GetError());
    }
    quiz->question = TTF_RenderText_Solid(font, quiz->questions[quiz->current_question].question, textColor);
    quiz->pos_question.x = 500;
    quiz->pos_question.y = 200;
    for (int i = 0; i < 3; i++) {
        char filename[20];
        sprintf(filename, "%c_normal.png", 'A' + i);
        quiz->answer_btn[i][0] = IMG_Load(filename);
        sprintf(filename, "%c_hover.png", 'A' + i);
        quiz->answer_btn[i][1] = IMG_Load(filename);
        quiz->answer_text[i] = TTF_RenderText_Solid(font, quiz->questions[quiz->current_question].answers[i], textColor);
        quiz->pos_answer_btn[i].x = 1035;
        quiz->pos_answer_btn[i].y = 650 + i * 100;
        quiz->pos_answer_text[i].x = quiz->pos_answer_btn[i].x + 130;
        quiz->pos_answer_text[i].y = quiz->pos_answer_btn[i].y + 10;
    }
    TTF_CloseFont(font);
}

void display_enigma(SDL_Surface *screen, bg background, bouton quiz_btn, bouton puzzle_btn, quiz_state quiz) {
    SDL_BlitSurface(background.T[0], NULL, screen, &background.pos_img_affiche);
    if (!quiz.active) {
        SDL_BlitSurface(quiz_btn.TB[quiz_btn.etat], NULL, screen, &quiz_btn.pos_img_ecran);
        SDL_BlitSurface(puzzle_btn.TB[puzzle_btn.etat], NULL, screen, &puzzle_btn.pos_img_ecran);
    } else {
        SDL_BlitSurface(quiz.question, NULL, screen, &quiz.pos_question);
        for (int i = 0; i < 3; i++) {
            SDL_BlitSurface(quiz.answer_btn[i][quiz.answer_hover_state[i]], NULL, screen, &quiz.pos_answer_btn[i]);
            SDL_BlitSurface(quiz.answer_text[i], NULL, screen, &quiz.pos_answer_text[i]);
        }
    }
    SDL_Flip(screen);
}

void handle_enigma_events(SDL_Event event, int *running, int *quiz_selected, int *puzzle_selected, bouton *quiz_btn, bouton *puzzle_btn, quiz_state *quiz, bg *background) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = 0;
                break;
            case SDL_MOUSEMOTION:
                if (quiz->active) {
                    for (int i = 0; i < 3; i++) {
                        if (event.motion.x >= quiz->pos_answer_btn[i].x && event.motion.x <= quiz->pos_answer_btn[i].x + 119 &&
                            event.motion.y >= quiz->pos_answer_btn[i].y && event.motion.y <= quiz->pos_answer_btn[i].y + 51) {
                            if (quiz->answer_hover_state[i] == 0) {
                                // Play hover sound only when the mouse enters the button
                                if (hover_sound) {
                                    Mix_PlayChannel(-1, hover_sound, 0);
                                }
                            }
                            quiz->answer_hover_state[i] = 1;
                        } else {
                            quiz->answer_hover_state[i] = 0;
                        }
                    }
                } else {
                    if (event.motion.x >= quiz_btn->pos_img_ecran.x && event.motion.x <= quiz_btn->pos_img_ecran.x + 322 &&
                        event.motion.y >= quiz_btn->pos_img_ecran.y && event.motion.y <= quiz_btn->pos_img_ecran.y + 188) {
                         if (quiz_btn->etat== 0) {
                                // Play hover sound only when the mouse enters the button
                                if (hover_sound) {
                                    Mix_PlayChannel(-1, hover_sound, 0);
                                }
                            }
                        quiz_btn->etat = 1;
                    } else {
                        quiz_btn->etat = 0;
                    }
                    if (event.motion.x >= puzzle_btn->pos_img_ecran.x && event.motion.x <= puzzle_btn->pos_img_ecran.x + 322 &&
                        event.motion.y >= puzzle_btn->pos_img_ecran.y && event.motion.y <= puzzle_btn->pos_img_ecran.y + 188) {
                         if (puzzle_btn->etat == 0) {
                                // Play hover sound only when the mouse enters the button
                                if (hover_sound) {
                                    Mix_PlayChannel(-1, hover_sound, 0);
                                }
                            }
                        puzzle_btn->etat = 1;
                    } else {
                        puzzle_btn->etat = 0;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (!quiz->active) {
                    if (event.button.x >= quiz_btn->pos_img_ecran.x && event.button.x <= quiz_btn->pos_img_ecran.x + 322 &&
                        event.button.y >= quiz_btn->pos_img_ecran.y && event.button.y <= quiz_btn->pos_img_ecran.y + 188) {
                        *quiz_selected = 1;
                        quiz->active = 1;
                        if (background->music) {
                            if (Mix_PlayingMusic() == 0) {
                                Mix_PlayMusic(background->music, -1);
                            }
                        }
                    }
                    if (event.button.x >= puzzle_btn->pos_img_ecran.x && event.button.x <= puzzle_btn->pos_img_ecran.x + 322 &&
                        event.button.y >= puzzle_btn->pos_img_ecran.y && event.button.y <= puzzle_btn->pos_img_ecran.y + 188) {
                        *puzzle_selected = 1;
                    }
                } else {
                    for (int i = 0; i < 3; i++) {
                        if (event.button.x >= quiz->pos_answer_btn[i].x && event.button.x <= quiz->pos_answer_btn[i].x + 119 &&
                            event.button.y >= quiz->pos_answer_btn[i].y && event.button.y <= quiz->pos_answer_btn[i].y + 51) {
                            printf("Answer %c selected!\n", 'A' + i);
                            if (i == quiz->questions[quiz->current_question].correct_answer) {
                                printf("Correct answer!\n");
                            } else {
                                printf("Wrong answer! The correct answer is %c.\n", 'A' + quiz->questions[quiz->current_question].correct_answer);
                            }
                            quiz->active = 0;
                            *quiz_selected = 0;
                            if (background->music) {
                                Mix_HaltMusic();
                            }
                            break;
                        }
                    }
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    *running = 0;
                }
                break;
        }
    }
}

void free_enigma(bg *background, bouton *quiz_btn, bouton *puzzle_btn, quiz_state *quiz) {
    SDL_FreeSurface(background->T[0]);
    SDL_FreeSurface(quiz_btn->TB[0]);
    SDL_FreeSurface(quiz_btn->TB[1]);
    SDL_FreeSurface(puzzle_btn->TB[0]);
    SDL_FreeSurface(puzzle_btn->TB[1]);
    SDL_FreeSurface(quiz->question);
    for (int i = 0; i < 3; i++) {
        SDL_FreeSurface(quiz->answer_btn[i][0]);
        SDL_FreeSurface(quiz->answer_btn[i][1]);
        SDL_FreeSurface(quiz->answer_text[i]);
    }
    Mix_FreeMusic(background->music);
    if (hover_sound) {
        Mix_FreeChunk(hover_sound);
        hover_sound = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}

int runEnigme(SDL_Surface *screen) {
    SDL_Event event;
    int running = 1;
    int quiz_selected = 0, puzzle_selected = 0;
    bg background;
    bouton quiz_btn, puzzle_btn;
    quiz_state quiz;

    init_enigma(&background, &quiz_btn, &puzzle_btn, &screen, &quiz);

    while (running) {
        handle_enigma_events(event, &running, &quiz_selected, &puzzle_selected, &quiz_btn, &puzzle_btn, &quiz, &background);
        display_enigma(screen, background, quiz_btn, puzzle_btn, quiz);
        SDL_Delay(16); // Approx. 60 FPS
    }
    free_enigma(&background, &quiz_btn, &puzzle_btn, &quiz);
    return 0;
}

