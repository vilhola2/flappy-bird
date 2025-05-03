#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL3/SDL.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} App;

typedef struct {
    SDL_Color color;
    SDL_FRect rect;
    float velocity;
} Player;

#endif

