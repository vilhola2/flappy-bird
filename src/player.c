#include <SDL3_mixer/SDL_mixer.h>
#include "structs.h"
#include "constants.h"
#include "globals.h"
#include "audio.h"
#include "obstacles.h"

void init_player(Player *player) {
    *player = (Player) { .color = { 0x4B, 0xFF, 0x4B, 0xFF }, .rect = { .x = PLAYER_X, .w = PLAYER_SIZE, .h = PLAYER_SIZE } };
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

void player_game_over(Player *player, SDL_Renderer *renderer) {
    Mix_PlayChannel(-1, explosion, false);
    player->color = (SDL_Color){ 0xFF, 0x4B, 0x4B, 0x4B };
    render_player(player, renderer);
    SDL_RenderPresent(renderer);
    init_player(player);
    init_obstacles();
    SDL_Delay(1000);
}
