#include <SDL3/SDL.h>
#include "constants.h"
#include "globals.h"
#include "structs.h"

#define MAX_BARS 128

typedef struct {
    SDL_FRect top_bar;
    SDL_FRect bottom_bar;
} Bars;

// only use these from main thread
static int bars_count = 0;
static Bars *bars_arr[MAX_BARS] = {0};

static SDL_AtomicInt spawn_requests;

bool spawn_bars(void) {
    if(bars_count > MAX_BARS) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Too many bars!\n");
        return false;
    }
    Bars *bars = SDL_malloc(sizeof(Bars));
    if(!bars) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate a set of bars\n");
        return false;
    }
    const int rand = SDL_rand(SCREEN_HEIGHT - PLAYER_SIZE);
    //SDL_Log("%d\n", rand);
    *bars = (Bars) {
        .top_bar = { .x = SCREEN_WIDTH, .w = PLAYER_SIZE * 2, .h = rand },
        .bottom_bar = { .x = SCREEN_WIDTH, .y = PLAYER_SIZE * 4 + rand, .w = PLAYER_SIZE * 2, .h = SCREEN_HEIGHT },
    };
    bars_arr[bars_count++] = bars;
    return true;
}

static SDL_TimerID bars_timer = 0;
Uint32 bars_callback(void* param, SDL_TimerID timer_id, Uint32 interval) {
    (void) param; (void) timer_id;
    SDL_AddAtomicInt(&spawn_requests, 1);
    return interval;
}

void init_obstacles(void) {
    for(int i = 0; i < bars_count; ++i) {
        SDL_free(bars_arr[i]);
        bars_arr[i] = nullptr;
    }
    bars_count = 0;
    SDL_SetAtomicInt(&spawn_requests, 0);
    if(bars_timer) SDL_RemoveTimer(bars_timer);
    bars_timer = SDL_AddTimer(3000, bars_callback, nullptr);
}

void destroy_obstacles(void) {
    if(bars_timer) SDL_RemoveTimer(bars_timer);
    for(int i = 0; i < bars_count; ++i) {
        SDL_free(bars_arr[i]);
        SDL_Log("free called\n");
    }
}

// returns true on collision
bool rects_collide(const SDL_FRect *a, const SDL_FRect *b) {
    return !(a->x + a->w < b->x ||
             a->x > b->x + b->w ||
             a->y + a->h < b->y ||
             a->y > b->y + b->h);
}

bool process_obstacles(Player *player, SDL_Renderer *renderer) {
    while(SDL_GetAtomicInt(&spawn_requests) > 0) {
        SDL_AddAtomicInt(&spawn_requests, -1);
        spawn_bars();
    }
    bool collision = false;
    SDL_SetRenderDrawColor(renderer, 0xAF, 0xAF, 0xAF, 0xFF);
    for(int i = 0; i < bars_count; ++i) {
        SDL_FRect *top_rect = &bars_arr[i]->top_bar;
        SDL_FRect *bottom_rect = &bars_arr[i]->bottom_bar;
        SDL_RenderFillRect(renderer, top_rect);
        SDL_RenderFillRect(renderer, bottom_rect);
        if(rects_collide(&player->rect, top_rect) || rects_collide(&player->rect, bottom_rect)) collision = true;
        top_rect->x -= 120 * delta_time;
        bottom_rect->x -= 120 * delta_time;
        if(top_rect->x <= -top_rect->w) {
            SDL_free(bars_arr[i]);
            SDL_Log("free called\n");
            for(int j = i; j < bars_count - 1; ++j) bars_arr[j] = bars_arr[j + 1];
            --bars_count;
            --i;
        }
    }
    return collision;
}

