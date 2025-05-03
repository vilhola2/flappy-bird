#include "structs.h"
#include "constants.h"
#include "globals.h"

void update_player_pos(Player *player) {
    player->velocity += PLAYER_GRAVITY;
    player->rect.y += player->velocity* delta_time;
    if(player->rect.y >= SCREEN_HEIGHT - PLAYER_SIZE) {
        player->rect.y = SCREEN_HEIGHT - PLAYER_SIZE;
        player->velocity = 0;
    }
}

void trigger_player_jump(Player *player) {
    player->velocity = -PLAYER_JUMP_HEIGHT;
}

