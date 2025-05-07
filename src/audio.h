#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3_mixer/SDL_mixer.h>

extern Mix_Chunk *jump, *explosion;
bool init_sounds(void);
void destroy_sounds(void);

#endif

