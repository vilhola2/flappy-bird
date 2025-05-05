#include "structs.h"
#include "constants.h"
#include "globals.h"

void init_player(Player *player) {
    *player = (Player) { .color = { 0xBB, 0xBB, 0xBB, 0xFF }, .rect = { .x = PLAYER_X, .w = PLAYER_SIZE, .h = PLAYER_SIZE } };
}

bool update_player_pos(Player *player) {
    player->velocity += PLAYER_GRAVITY;
    player->rect.y += player->velocity* delta_time;
    if(player->rect.y < 0) return true;
    if(player->rect.y >= SCREEN_HEIGHT - PLAYER_SIZE) {
        player->rect.y = SCREEN_HEIGHT - PLAYER_SIZE;
        player->velocity = 0;
    }
    return false;
}

void trigger_player_jump(Player *player) {
    player->velocity = -PLAYER_JUMP_HEIGHT;
}

void render_player(Player *player, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, player->color.r, player->color.g, player->color.b, player->color.a);
    SDL_RenderFillRect(renderer, &(player->rect));
}
