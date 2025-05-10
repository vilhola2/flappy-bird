#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "structs.h"

#define SCREEN_STARTING_WIDTH 800
#define SCREEN_STARTING_HEIGHT 600
Sint32 screen_width = SCREEN_STARTING_WIDTH;
Sint32 screen_height = SCREEN_STARTING_HEIGHT;
float player_x = 0;

bool init_sdl(App *a) {
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL: %s", SDL_GetError());
        return false;
    }
    if(!TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL_TTF: %s", SDL_GetError());
        return false;
    }
    if(!(a->window = SDL_CreateWindow("Flappy bird", screen_width, screen_height, SDL_WINDOW_RESIZABLE))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s", SDL_GetError());
        return false;
    }
    if(!SDL_SetWindowMinimumSize(a->window, 800, 600)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to set minimum window size: %s", SDL_GetError());
        return false;
    }
    if(!(a->renderer = SDL_CreateRenderer(a->window, nullptr))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create renderer: %s", SDL_GetError());
        return false;
    }
    if(!SDL_SetRenderVSync(a->renderer, 1)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to enable VSync: %s\n", SDL_GetError());
        return false;
    }
    if(!Mix_OpenAudio(0, &(SDL_AudioSpec) {
        .format = MIX_DEFAULT_FORMAT,
        .channels = MIX_DEFAULT_CHANNELS,
        .freq = MIX_DEFAULT_FREQUENCY
    })) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL3_mixer: %s", SDL_GetError());
    }
    return true;
}
