#ifndef PLAYER_H
#define PLAYER_H

#include "structs.h"

bool init_player(Player *player);

// returns true on collision
bool update_player_pos(Player *player);

void trigger_player_jump(Player *player);
void render_player(Player *player, SDL_Renderer *renderer);
void player_game_over(Player *player, SDL_Renderer *renderer);
void update_score(Player *player);
void init_score_timer(void);
void render_score(SDL_Renderer *renderer);
void destroy_score(void);
void player_cleanup(void);

#endif

