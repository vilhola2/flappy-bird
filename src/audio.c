#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

Mix_Chunk *jump = nullptr, *explosion = nullptr;

bool init_sounds(void) {
    bool error = false;

    if(!(jump = Mix_LoadWAV("resources/sounds/jump.wav"))) error = true;
    else if(!(explosion = Mix_LoadWAV("resources/sounds/explosion.wav"))) error = true;

    if(error) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load WAV: %s", SDL_GetError());
        return false;
    } else return true;
}

void destroy_sounds(void) {
    if(jump) Mix_FreeChunk(jump);
    if(explosion) Mix_FreeChunk(explosion);
}

