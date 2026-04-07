#pragma once

// ============================================================
// Megaman99 - Game Constants
// ============================================================

// Screen dimensions (Miyoo Mini native: 640x480)
#ifdef PLATFORM_MIYOO
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
#else
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
#endif

// Internal game resolution (classic NES-style)
constexpr int GAME_WIDTH = 256;
constexpr int GAME_HEIGHT = 240;

// Tile size
constexpr int TILE_SIZE = 16;

// Map dimensions (in tiles)
constexpr int MAP_COLS = 64;
constexpr int MAP_ROWS = 15; // 240 / 16

// Physics
constexpr float GRAVITY = 0.45f;
constexpr float MAX_FALL_SPEED = 7.0f;
constexpr float PLAYER_SPEED = 1.8f;
constexpr float PLAYER_JUMP_FORCE = -6.5f;
constexpr float BULLET_SPEED = 5.0f;

// Player
constexpr int PLAYER_WIDTH = 16;
constexpr int PLAYER_HEIGHT = 24;
constexpr int PLAYER_MAX_HP = 28;
constexpr int MAX_BULLETS = 3;

// Enemy types
constexpr int ENEMY_MET = 0;      // Met (hard hat enemy)
constexpr int ENEMY_SNIPER = 1;   // Sniper Joe
constexpr int ENEMY_FLYING = 2;   // Flying enemy
constexpr int ENEMY_BOSS = 3;     // Stage boss

// Game states
constexpr int STATE_TITLE = 0;
constexpr int STATE_STAGE_SELECT = 1;
constexpr int STATE_PLAYING = 2;
constexpr int STATE_PAUSED = 3;
constexpr int STATE_GAME_OVER = 4;
constexpr int STATE_STAGE_CLEAR = 5;

// Colors (RGBA)
constexpr int COL_BG_R = 0;
constexpr int COL_BG_G = 0;
constexpr int COL_BG_B = 0;

constexpr int COL_MEGA_R = 0;
constexpr int COL_MEGA_G = 120;
constexpr int COL_MEGA_B = 248;

constexpr int COL_MEGA_LIGHT_R = 104;
constexpr int COL_MEGA_LIGHT_G = 184;
constexpr int COL_MEGA_LIGHT_B = 252;

constexpr int COL_HP_R = 248;
constexpr int COL_HP_G = 56;
constexpr int COL_HP_B = 0;

// FPS
constexpr int TARGET_FPS = 60;
constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

// Input (Miyoo Mini button mapping)
#ifdef PLATFORM_MIYOO
// Miyoo Mini uses standard SDL keycodes mapped to buttons
constexpr int BTN_A = SDLK_SPACE;      // A button
constexpr int BTN_B = SDLK_LCTRL;      // B button
constexpr int BTN_X = SDLK_LSHIFT;     // X button
constexpr int BTN_Y = SDLK_LALT;       // Y button
constexpr int BTN_START = SDLK_RETURN;  // Start
constexpr int BTN_SELECT = SDLK_RCTRL;  // Select
constexpr int BTN_L = SDLK_TAB;         // L shoulder
constexpr int BTN_R = SDLK_BACKSPACE;   // R shoulder
constexpr int BTN_UP = SDLK_UP;
constexpr int BTN_DOWN = SDLK_DOWN;
constexpr int BTN_LEFT = SDLK_LEFT;
constexpr int BTN_RIGHT = SDLK_RIGHT;
constexpr int BTN_MENU = SDLK_ESCAPE;   // Menu button
#endif

// Animation
constexpr int ANIM_SPEED_RUN = 6;     // Frames per animation frame
constexpr int ANIM_SPEED_SHOOT = 12;
constexpr int INVINCIBILITY_FRAMES = 90;
constexpr int DEATH_ANIMATION_FRAMES = 60;
