#include <SDL3_ttf/SDL_ttf.h>
#include "structs.h"
#include "constants.h"
#include "globals.h"
#include "audio.h"
#include "obstacles.h"
#include "player.h"

SDL_Mutex *player_mutex = nullptr;
static SDL_AtomicInt score_requests;
static char *score = nullptr;

bool init_player(Player *player) {
    *player = (Player) { .color = { 0x4B, 0xFF, 0x4B, 0xFF }, .rect = { .x = player_x, .w = PLAYER_SIZE, .h = PLAYER_SIZE } };
    if(!player_mutex) {
        if(!(player_mutex = SDL_CreateMutex())) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create player_mutex: %s\n", SDL_GetError());
            return false;
        }
    }
    if(score && *score != '0') {
        SDL_free(score);
        score = nullptr;
    }
    init_score_timer();
    return true;
}

bool update_player_pos(Player *player) {
    bool collision = false;
    SDL_LockMutex(player_mutex);
    player->velocity += PLAYER_GRAVITY * delta_time;
    player->rect.y += player->velocity * delta_time;
    if(player->rect.y < 0) {
        player->rect.y = 0;
        collision = true;
    } else if(player->rect.y >= screen_height - PLAYER_SIZE) {
        player->rect.y = screen_height - PLAYER_SIZE;
        collision = true;
    }
    SDL_UnlockMutex(player_mutex);
    return collision;
}

void trigger_player_jump(Player *player) {
    SDL_LockMutex(player_mutex);
    player->velocity = -PLAYER_JUMP_VELOCITY;
    SDL_UnlockMutex(player_mutex);
}

void render_player(Player *player, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, player->color.r, player->color.g, player->color.b, player->color.a);
    SDL_RenderFillRect(renderer, &(player->rect));
}

void player_game_over(Player *player, SDL_Renderer *renderer) {
    Mix_PlayChannel(-1, explosion, false);
    player->color = (SDL_Color) { 0xFF, 0x4B, 0x4B, 0x4B };
    render_player(player, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
    init_player(player);
    init_obstacles();
}


static inline void add_score(Player *player) { player->score += 25; }

Uint32 score_callback(void* param, SDL_TimerID timer_id, Uint32 interval) { 
    (void) param; (void) timer_id;
    SDL_AddAtomicInt(&score_requests, 1);
    return interval;
}

static SDL_TimerID bars_timer = 0;
void init_score_timer(void) {
    SDL_SetAtomicInt(&score_requests, 0);
    if(bars_timer) {
        SDL_RemoveTimer(bars_timer);
    }
    bars_timer = SDL_AddTimer(2000, score_callback, nullptr);
}

void update_score(Player *player) {
    while(SDL_GetAtomicInt(&score_requests) > 0) {
        SDL_AddAtomicInt(&score_requests, -1);
        add_score(player);
        if(score && *score != '0') SDL_free(score);
        SDL_asprintf(&score, "%d", player->score);
        //SDL_Log("%s\n", score);
    }
}

void destroy_score(void) {
    if(score) SDL_free(score);
}

void player_cleanup(void) {
    if(player_mutex) SDL_DestroyMutex(player_mutex);
}

void render_score(SDL_Renderer *renderer) {
    char *text = (score ? score : "0");
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, (SDL_Color){ 0xFF, 0xFF, 0xFF, 0xFF });
        if(!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_RenderText_Solid failed: %s", SDL_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FRect dst = { (float)screen_width / 2 - (float)surface->w / 10 / 2, 0, (float)surface->w / 10, (float)surface->h / 10};
    SDL_DestroySurface(surface);
    if(!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return;
    }
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

