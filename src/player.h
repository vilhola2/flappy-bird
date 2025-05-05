#ifndef PLAYER_H
#define PLAYER_H

#include "structs.h"

void init_player(Player *player);

// returns true on collision
bool update_player_pos(Player *player);

void trigger_player_jump(Player *player);
void render_player(Player *player, SDL_Renderer *renderer);
void player_game_over(Player *player, SDL_Renderer *renderer);

#endif

