#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

Mix_Chunk *jump = nullptr, *explosion = nullptr;
bool init_sounds(void) {
    if(!(jump = Mix_LoadWAV("resources/jump.wav"))) return false;
    if(!(explosion = Mix_LoadWAV("resources/explosion.wav"))) return false;
    return true;
}

void destroy_sounds(void) {
    if(jump) Mix_FreeChunk(jump);
}

