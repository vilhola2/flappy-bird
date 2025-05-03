#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <SDL3/SDL.h>
#include "structs.h"

void init_obstacles(void);
void destroy_obstacles(void);
bool process_obstacles(Player *player, SDL_Renderer *renderer);

typedef struct Bars Bars;
typedef bool(*spawn_bars_ptr)(void);

#endif

