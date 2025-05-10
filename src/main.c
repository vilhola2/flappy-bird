#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "structs.h"
#include "init.h"
#include "player.h"
#include "audio.h"
#include "obstacles.h"
#include "globals.h"

Player player;

float delta_time = 0;

SDL_AtomicInt new_width, new_height;
void update_screen_variables(SDL_Renderer *renderer) {
    int actual_width = SDL_GetAtomicInt(&new_width);
    int actual_height = SDL_GetAtomicInt(&new_height);
    SDL_SetAtomicInt(&new_width, 0);
    SDL_SetAtomicInt(&new_height, 0);
    float scale = (float)actual_height / (float)screen_height;
    screen_width = (int)((float)actual_width / scale);
    SDL_SetRenderScale(renderer, scale, scale);
    player_x = (float)screen_width / 2.0f - (float)screen_width / 4.0f;
    SDL_Log("actual: %dx%d -> virtual: %dx%d, scale: %f\n", actual_width, actual_height, screen_width, screen_height, scale);
}


void update_delta_time(void) {
    static Uint64 last = 0;
    Uint64 now = SDL_GetPerformanceCounter();
    if(!last) {
        last = now;
        delta_time = 1.0f / 60;
        return;
    }
    delta_time = (now - last) / (float)SDL_GetPerformanceFrequency();
    last = now;
    if(delta_time > 0.05f) delta_time = 0.05f; // clamp large jumps
}

TTF_Font *font = nullptr;
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    (void) argc;
    (void) argv;
    static App a;
    *appstate = &a;
    SDL_srand(0);
    if(!init_sdl(&a)) return SDL_APP_FAILURE;
    if(!init_sounds()) return SDL_APP_FAILURE;
    SDL_SetAtomicInt(&new_width, 800);
    SDL_SetAtomicInt(&new_height, 600);
    update_screen_variables(a.renderer);
    init_player(&player);
    init_score_timer();
    init_obstacles();
    font = TTF_OpenFont("resources/fonts/IBM-VGA-8x14.ttf", 600);
    if(!font) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open font: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
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
                jump_key_was_down = false;
            }
            break;
        case SDL_EVENT_KEY_DOWN:
            switch(event->key.key) {
                case SDLK_SPACE: case SDLK_UP:
                    if(!jump_key_was_down) {
                        Mix_PlayChannel(-1, jump, false);
                        trigger_player_jump(&player);
                    }
                    jump_key_was_down = true;
            }
            break;
        case SDL_EVENT_KEY_UP:
            switch(event->key.key) {
                case SDLK_SPACE: case SDLK_UP:
                    jump_key_was_down = false;
            }
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            SDL_SetAtomicInt(&new_width, event->window.data1);
            SDL_SetAtomicInt(&new_height, event->window.data2);
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    App *a = appstate;
    if(SDL_GetAtomicInt(&new_width) || SDL_GetAtomicInt(&new_height)) update_screen_variables(a->renderer);
    update_delta_time();
    SDL_SetRenderDrawColor(a->renderer, 0x0F, 0x0F, 0x0F, 0xFF);
    SDL_RenderClear(a->renderer);
    bool player_collided = false;
    player_collided |= update_player_pos(&player);
    render_player(&player, a->renderer);
    player_collided |= process_obstacles(&player, a->renderer);
    update_score(&player);
    render_score(a->renderer);
    if(player_collided) player_game_over(&player, a->renderer);
    SDL_RenderPresent(a->renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    App *a = appstate;
    if(result != SDL_APP_SUCCESS) SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Application failed to run\n");
    if(a->renderer) SDL_DestroyRenderer(a->renderer);
    if(a->window) SDL_DestroyWindow(a->window);
    destroy_score();
    destroy_sounds();
    destroy_obstacles();
    player_cleanup();
    Mix_CloseAudio();
    SDL_Quit();
    TTF_CloseFont(font);
}

