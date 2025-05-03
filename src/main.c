#include <SDL3/SDL_oldnames.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "structs.h"
#include "constants.h"
#include "init.h"
#include "player.h"
#include "audio.h"
#include "obstacles.h"

Player player;
void init_player(Player *player) {
    *player = (Player){ .color = { 0xBB, 0xBB, 0xBB, 0xFF }, .rect = { .x = PLAYER_X, .w = PLAYER_SIZE, .h = PLAYER_SIZE } };
}

void render_player(Player *player, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, player->color.r, player->color.g, player->color.b, player->color.a);
    SDL_RenderFillRect(renderer, &(player->rect));
}

float delta_time = 0;
void update_delta_time(void) {
    static Uint64 last_tick = 0, current_tick = 0;
    last_tick = current_tick;
    current_tick = SDL_GetTicks();
    delta_time = (current_tick - last_tick) / 1000.0f; // 1 second
    //SDL_Log("delta_time: %f\n", delta_time);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    (void) argc;
    (void) argv;
    static App a;
    *appstate = &a;
    SDL_srand(0);
    if(!init_sdl(&a)) return SDL_APP_FAILURE;
    if(!init_sounds()) return SDL_APP_FAILURE;
    init_player(&player);
    init_obstacles();
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    (void) appstate;
    static bool jump_key_was_down = false;
    switch(event->type) {
        case SDL_EVENT_QUIT: { return SDL_APP_SUCCESS; }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if(!jump_key_was_down && event->button.button == SDL_BUTTON_LEFT) {
                Mix_PlayChannel(-1, jump, false);
                trigger_player_jump(&player);
            }
            jump_key_was_down = true; break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if(event->button.button == SDL_BUTTON_LEFT) {
                jump_key_was_down = false; break;
            }
        case SDL_EVENT_KEY_DOWN:
            switch(event->key.key) {
                case SDLK_SPACE: case SDLK_UP:
                    if(!jump_key_was_down) {
                        Mix_PlayChannel(-1, jump, false);
                        trigger_player_jump(&player);
                    }
                    jump_key_was_down = true; break;
            }
            break;
        case SDL_EVENT_KEY_UP:
            switch(event->key.key) {
                case SDLK_SPACE: case SDLK_UP:
                    jump_key_was_down = false;
            }
        case SDL_EVENT_USER:
            switch(event->user.code) {
                case SPAWN_BARS: {
                    spawn_bars_ptr spawn_bars = (spawn_bars_ptr)event->user.data1; // user_data is a function pointer
                    if(!spawn_bars()) { return SDL_APP_FAILURE; } break;
                }
            }
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    App *a = appstate;
    update_delta_time();
    SDL_SetRenderDrawColor(a->renderer, 0x0F, 0x0F, 0x0F, 0xFF);
    SDL_RenderClear(a->renderer);
    update_player_pos(&player);
    render_player(&player, a->renderer);
    if(process_obstacles(&player, a->renderer)) {
        // collision happened
        Mix_PlayChannel(-1, explosion, false);
        player.color = (SDL_Color){ 0xD4, 0x4F, 0x4F, 0xFF };
        render_player(&player, a->renderer);
        SDL_RenderPresent(a->renderer);
        SDL_Delay(1000);
        init_player(&player);
        init_obstacles();
    }
    SDL_RenderPresent(a->renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    App *a = appstate;
    if(result != SDL_APP_SUCCESS) SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run\n");
    if(a->renderer) SDL_DestroyRenderer(a->renderer);
    if(a->window) SDL_DestroyWindow(a->window);
    destroy_sounds();
    destroy_obstacles();
    Mix_CloseAudio();
    SDL_Quit();
}

