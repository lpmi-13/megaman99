#pragma once

#include <SDL2/SDL.h>

// Abstracted input for both keyboard (native) and Miyoo Mini buttons
class Input {
public:
    enum Button {
        BTN_LEFT = 0,
        BTN_RIGHT,
        BTN_UP,
        BTN_DOWN,
        BTN_JUMP,    // A / Space
        BTN_SHOOT,   // B / Z
        BTN_START,   // Start / Enter
        BTN_SELECT,  // Select / Tab
        BTN_PAUSE,   // Menu / Escape
        BTN_COUNT
    };

    void update() {
        for (int i = 0; i < BTN_COUNT; i++) {
            prev_state[i] = curr_state[i];
        }
    }

    void handleEvent(const SDL_Event& e) {
        bool pressed = (e.type == SDL_KEYDOWN);
        bool released = (e.type == SDL_KEYUP);
        if (!pressed && !released) return;

        bool state = pressed;
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:      curr_state[BTN_LEFT] = state; break;
            case SDLK_RIGHT:     curr_state[BTN_RIGHT] = state; break;
            case SDLK_UP:        curr_state[BTN_UP] = state; break;
            case SDLK_DOWN:      curr_state[BTN_DOWN] = state; break;
            case SDLK_SPACE:
            case SDLK_LCTRL:     curr_state[BTN_JUMP] = state; break;
            case SDLK_z:
            case SDLK_LALT:      curr_state[BTN_SHOOT] = state; break;
            case SDLK_RETURN:    curr_state[BTN_START] = state; break;
            case SDLK_TAB:
            case SDLK_RCTRL:     curr_state[BTN_SELECT] = state; break;
            case SDLK_ESCAPE:    curr_state[BTN_PAUSE] = state; break;
            default: break;
        }
    }

    bool isDown(Button b) const { return curr_state[b]; }
    bool isPressed(Button b) const { return curr_state[b] && !prev_state[b]; }
    bool isReleased(Button b) const { return !curr_state[b] && prev_state[b]; }

private:
    bool curr_state[BTN_COUNT] = {};
    bool prev_state[BTN_COUNT] = {};
};
