#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define MIN_LEVEL 0
#define MAX_LEVEL 7
#define NUM_LEVELS (MAX_LEVEL - MIN_LEVEL + 1)

#define WIN_WIDTH  420
#define WIN_HEIGHT 160

#define TRACK_X 40
#define TRACK_Y 80
#define TRACK_W (WIN_WIDTH - 2 * TRACK_X)
#define TRACK_H 8

#define HANDLE_W 18
#define HANDLE_H 34

int set_fan_level(int level) {

    return level;

}

static int level_to_x(int level) {
    
    if (NUM_LEVELS <= 1) return TRACK_X;
    
    int usable = TRACK_W - HANDLE_W;
    
    return TRACK_X + (usable * level) / (NUM_LEVELS - 1);
}

static int x_to_level(int mouse_x) {
    
    int usable = TRACK_W - HANDLE_W;
    
    int rel = mouse_x - HANDLE_W / 2 - TRACK_X;
    
    if (rel < 0) rel = 0;
    if (rel > usable) rel = usable;
   

    int level = (rel * (NUM_LEVELS - 1) + usable / 2) / usable;
   
    if (level < MIN_LEVEL) level = MIN_LEVEL;
    if (level > MAX_LEVEL) level = MAX_LEVEL;
   
    return level;
}

static void update_title(SDL_Window *win, int level, bool write_failed) {
   
    char title[64];

    if (write_failed)
   
        snprintf(title, sizeof(title), "fan Level: %d  (root ok?)", level);
    
    else
        snprintf(title, sizeof(title), "fan Level: %d", level);

    SDL_SetWindowTitle(win, title);
}

static void fill_rect(SDL_Renderer *r, int x, int y, int w, int h, Uint8 red, Uint8 g, Uint8 b, Uint8 a) {
  
    SDL_SetRenderDrawColor(r, red, g, b, a);
  
    SDL_Rect rect = { x, y, w, h };
  
    SDL_RenderFillRect(r, &rect);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Lenevo Fan control",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!ren) {
        
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    int level = MIN_LEVEL;
    bool dragging = false;
    bool write_failed = false;
    bool running = true;


    write_failed = (set_fan_level(level) != 0);
    
    if (write_failed){
        fprintf(stderr, "root need");
    }
        
        
    
    
    update_title(win, level, write_failed);

    while (running) {
       
        SDL_Event e;
       
        while (SDL_PollEvent(&e)) {
       
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:

                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    
                        running = false;

                    else if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_DOWN) {
                       
                        if (level > MIN_LEVEL) level--;
                        
                        goto apply_level;
                    
                    } else if (e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_UP) {
                      
                        if (level < MAX_LEVEL) level++;
                        
                        goto apply_level;
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN: {
                    int mx = e.button.x, my = e.button.y;
                    int hx = level_to_x(level);
                    
                    SDL_Rect handle = { hx, TRACK_Y + TRACK_H / 2 - HANDLE_H / 2,
                                         HANDLE_W, HANDLE_H };
                    
                                         SDL_Rect track_hit = { TRACK_X, TRACK_Y - 20, TRACK_W, TRACK_H + 40 };
                    if ((mx >= handle.x && mx < handle.x + handle.w &&
                         my >= handle.y && my < handle.y + handle.h) ||
                        (mx >= track_hit.x && mx < track_hit.x + track_hit.w &&
                         my >= track_hit.y && my < track_hit.y + track_hit.h)) {
                    
                            dragging = true;
                    
                            int new_level = x_to_level(mx);
                    
                            if (new_level != level) {
                                level = new_level;
                                goto apply_level;
                            }
                    }
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                    dragging = false;
                    break;

                case SDL_MOUSEMOTION:
                    if (dragging) {
                        int new_level = x_to_level(e.motion.x);
                        if (new_level != level) {
                            level = new_level;
                            goto apply_level;
                        }
                    }
                    break;
            }
            continue;

        apply_level:
            write_failed = (set_fan_level(level) != 0);
            if (write_failed)
                fprintf(stderr, "warning: set_fan_level(%d) failed\n", level);
            update_title(win, level, write_failed);
        }

        fill_rect(ren, 0, 0, WIN_WIDTH, WIN_HEIGHT, 30, 30, 34, 255);

        
        for (int lv = MIN_LEVEL; lv <= MAX_LEVEL; lv++) {
            int tick_x = TRACK_X + HANDLE_W / 2 +
                         ((TRACK_W - HANDLE_W) * lv) / (NUM_LEVELS - 1) - 1;
            fill_rect(ren, tick_x, TRACK_Y + TRACK_H + 6, 2, 10, 90, 90, 96, 255);
        }

        fill_rect(ren, TRACK_X, TRACK_Y, TRACK_W, TRACK_H, 60, 60, 68, 255);

        int hx = level_to_x(level);
        int fill_w = hx + HANDLE_W / 2 - TRACK_X;
        fill_rect(ren, TRACK_X, TRACK_Y, fill_w, TRACK_H, 70, 160, 220, 255);



        fill_rect(ren, hx, TRACK_Y + TRACK_H / 2 - HANDLE_H / 2,
                   HANDLE_W, HANDLE_H, 230, 230, 235, 255);

        
                   
        for (int i = 0; i < NUM_LEVELS; i++) {
            Uint8 shade = (i < level) ? 220 : 55;
            fill_rect(ren, TRACK_X + i * 20, TRACK_Y - 30, 14, 14, shade, shade, shade, 255);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}