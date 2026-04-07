#pragma once

#include <SDL2/SDL.h>
#include <cstdio>
#include "constants.h"

class Renderer {
public:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* framebuffer = nullptr;

    bool init() {
        Uint32 flags = SDL_WINDOW_SHOWN;
#ifdef PLATFORM_MIYOO
        flags |= SDL_WINDOW_FULLSCREEN;
#endif
        window = SDL_CreateWindow("Megaman99",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT, flags);
        if (!window) {
            fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            // Fallback to software renderer (Miyoo Mini compatibility)
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        }
        if (!renderer) {
            fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
            return false;
        }

        // Create internal framebuffer at game resolution
        framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);
        if (!framebuffer) {
            fprintf(stderr, "Framebuffer creation failed: %s\n", SDL_GetError());
            return false;
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_ShowCursor(SDL_DISABLE);

        return true;
    }

    void beginFrame() {
        SDL_SetRenderTarget(renderer, framebuffer);
        SDL_SetRenderDrawColor(renderer, COL_BG_R, COL_BG_G, COL_BG_B, 255);
        SDL_RenderClear(renderer);
    }

    void endFrame() {
        // Render framebuffer scaled to screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, framebuffer, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void drawRect(int x, int y, int w, int h, int r, int g, int b, int a = 255) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer, &rect);
    }

    void drawRectOutline(int x, int y, int w, int h, int r, int g, int b) {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderDrawRect(renderer, &rect);
    }

    void shutdown() {
        if (framebuffer) SDL_DestroyTexture(framebuffer);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
    }
};
