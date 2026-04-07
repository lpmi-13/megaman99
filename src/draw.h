#pragma once

#include <SDL2/SDL.h>
#include "renderer.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "particle.h"
#include "level.h"
#include "constants.h"

// Software sprite drawing - no external assets needed
// Draws everything using colored rectangles (NES-style)
namespace Draw {

    inline void pixel(Renderer& ren, int x, int y, int r, int g, int b) {
        ren.drawRect(x, y, 1, 1, r, g, b);
    }

    // Draw Megaman sprite using rectangles
    inline void megaman(Renderer& ren, const Player& player, int camX) {
        if (!player.alive) return;

        // Blink when invincible
        if (player.invincibleTimer > 0 && (player.invincibleTimer / 3) % 2 == 0) return;

        int x = static_cast<int>(player.pos.x) - camX;
        int y = static_cast<int>(player.pos.y);
        bool right = player.facingRight;

        int bodyR = COL_MEGA_R, bodyG = COL_MEGA_G, bodyB = COL_MEGA_B;
        int lightR = COL_MEGA_LIGHT_R, lightG = COL_MEGA_LIGHT_G, lightB = COL_MEGA_LIGHT_B;
        int skinR = 252, skinG = 196, skinB = 160;

        // Head (helmet)
        ren.drawRect(x + 3, y, 10, 3, lightR, lightG, lightB);
        ren.drawRect(x + 2, y + 3, 12, 4, bodyR, bodyG, bodyB);

        // Face
        if (right) {
            ren.drawRect(x + 9, y + 3, 4, 4, skinR, skinG, skinB);
            // Eye
            ren.drawRect(x + 11, y + 4, 2, 2, 20, 20, 20);
        } else {
            ren.drawRect(x + 3, y + 3, 4, 4, skinR, skinG, skinB);
            ren.drawRect(x + 3, y + 4, 2, 2, 20, 20, 20);
        }

        // Body
        ren.drawRect(x + 4, y + 7, 8, 6, bodyR, bodyG, bodyB);
        // Body highlight
        ren.drawRect(x + 5, y + 8, 3, 4, lightR, lightG, lightB);

        // Arms
        bool shootPose = player.shootTimer > 0;
        if (shootPose) {
            // Extended arm (buster)
            if (right) {
                ren.drawRect(x + 12, y + 8, 6, 4, bodyR, bodyG, bodyB);
                ren.drawRect(x + 16, y + 7, 3, 6, lightR, lightG, lightB);
            } else {
                ren.drawRect(x - 2, y + 8, 6, 4, bodyR, bodyG, bodyB);
                ren.drawRect(x - 3, y + 7, 3, 6, lightR, lightG, lightB);
            }
        } else {
            // Normal arms
            ren.drawRect(x + 1, y + 8, 3, 4, bodyR, bodyG, bodyB);
            ren.drawRect(x + 12, y + 8, 3, 4, bodyR, bodyG, bodyB);
        }

        // Legs
        int legOffset = 0;
        if (player.animFrame == 1) legOffset = 1;
        else if (player.animFrame == 2) legOffset = -1;

        if (!player.onGround) {
            // Jump pose - legs spread
            ren.drawRect(x + 3, y + 13, 4, 5, bodyR, bodyG, bodyB);
            ren.drawRect(x + 9, y + 13, 4, 5, bodyR, bodyG, bodyB);
            // Boots
            ren.drawRect(x + 2, y + 18, 5, 6, lightR, lightG, lightB);
            ren.drawRect(x + 9, y + 18, 5, 6, lightR, lightG, lightB);
        } else {
            // Running / idle legs
            ren.drawRect(x + 4 + legOffset, y + 13, 3, 5, bodyR, bodyG, bodyB);
            ren.drawRect(x + 9 - legOffset, y + 13, 3, 5, bodyR, bodyG, bodyB);
            // Boots
            ren.drawRect(x + 3 + legOffset, y + 18, 4, 6, lightR, lightG, lightB);
            ren.drawRect(x + 9 - legOffset, y + 18, 4, 6, lightR, lightG, lightB);
        }
    }

    // Draw enemy sprite
    inline void enemy(Renderer& ren, const Enemy& e, int camX) {
        if (!e.active) return;

        int x = static_cast<int>(e.pos.x) - camX;
        int y = static_cast<int>(e.pos.y);

        switch (e.type) {
            case ENEMY_MET: {
                // Met (hard hat) - yellow helmet enemy
                if (e.state == 0) {
                    // Hidden under helmet
                    ren.drawRect(x + 1, y + 4, 14, 8, 200, 180, 0);
                    ren.drawRect(x + 3, y + 2, 10, 4, 240, 220, 40);
                    ren.drawRect(x + 2, y + 12, 12, 4, 100, 80, 0);
                } else {
                    // Exposed
                    ren.drawRect(x + 3, y, 10, 4, 240, 220, 40);  // Hat top
                    ren.drawRect(x + 2, y + 4, 12, 4, 60, 60, 60); // Face
                    ren.drawRect(x + 4, y + 5, 2, 2, 255, 255, 255); // Eyes
                    ren.drawRect(x + 10, y + 5, 2, 2, 255, 255, 255);
                    ren.drawRect(x + 2, y + 8, 12, 4, 200, 180, 0); // Body
                    ren.drawRect(x + 3, y + 12, 10, 4, 100, 80, 0); // Feet
                }
                break;
            }
            case ENEMY_SNIPER: {
                // Sniper Joe - green armored enemy
                int gr = 40, gg = 160, gb = 40;
                // Body
                ren.drawRect(x + 2, y, 12, 6, gr, gg, gb);
                // Visor
                int vx = e.facingRight ? x + 9 : x + 3;
                ren.drawRect(vx, y + 2, 4, 3, 255, 60, 60);
                // Torso
                ren.drawRect(x + 1, y + 6, 14, 8, gr, gg, gb);
                // Shield
                if (!e.facingRight) {
                    ren.drawRect(x - 1, y + 4, 4, 12, 100, 100, 100);
                } else {
                    ren.drawRect(x + 13, y + 4, 4, 12, 100, 100, 100);
                }
                // Legs
                ren.drawRect(x + 3, y + 14, 4, 6, gr - 10, gg - 20, gb - 10);
                ren.drawRect(x + 9, y + 14, 4, 6, gr - 10, gg - 20, gb - 10);
                // Boots
                ren.drawRect(x + 2, y + 20, 5, 4, 60, 60, 60);
                ren.drawRect(x + 9, y + 20, 5, 4, 60, 60, 60);
                break;
            }
            case ENEMY_FLYING: {
                // Flying enemy - red propeller enemy
                int fr = 220, fg = 60, fb = 60;
                // Propeller
                int propW = (e.animFrame % 2 == 0) ? 12 : 4;
                ren.drawRect(x + 8 - propW / 2, y - 2, propW, 2, 180, 180, 180);
                ren.drawRect(x + 6, y, 4, 3, 100, 100, 100); // Shaft
                // Body
                ren.drawRect(x + 2, y + 3, 12, 8, fr, fg, fb);
                // Eye
                int ex = e.facingRight ? x + 9 : x + 4;
                ren.drawRect(ex, y + 5, 3, 3, 255, 255, 255);
                ren.drawRect(ex + 1, y + 6, 1, 1, 0, 0, 0);
                // Bottom
                ren.drawRect(x + 4, y + 11, 8, 5, fr - 40, fg - 20, fb - 20);
                break;
            }
            case ENEMY_BOSS: {
                // Boss - large purple armored enemy
                int br = 160, bg2 = 40, bb = 200;
                // Helmet
                ren.drawRect(x + 4, y, 24, 6, br, bg2, bb);
                ren.drawRect(x + 8, y - 4, 16, 6, br + 30, bg2 + 20, bb);
                // Face
                ren.drawRect(x + 8, y + 6, 16, 8, 200, 160, 140);
                // Eyes
                ren.drawRect(x + 10, y + 8, 4, 4, 255, 60, 60);
                ren.drawRect(x + 18, y + 8, 4, 4, 255, 60, 60);
                // Body
                ren.drawRect(x + 2, y + 14, 28, 10, br, bg2, bb);
                ren.drawRect(x + 6, y + 16, 20, 6, br + 20, bg2 + 10, bb);
                // Arms
                ren.drawRect(x - 2, y + 14, 6, 10, br - 20, bg2, bb - 20);
                ren.drawRect(x + 28, y + 14, 6, 10, br - 20, bg2, bb - 20);
                // Legs
                ren.drawRect(x + 4, y + 24, 8, 8, br - 20, bg2, bb - 20);
                ren.drawRect(x + 20, y + 24, 8, 8, br - 20, bg2, bb - 20);
                break;
            }
        }
    }

    // Draw a bullet
    inline void bullet(Renderer& ren, const Bullet& b, int camX) {
        if (!b.active) return;
        int x = static_cast<int>(b.pos.x) - camX;
        int y = static_cast<int>(b.pos.y);
        if (b.fromPlayer) {
            ren.drawRect(x, y, b.width, b.height, 248, 248, 80);
            ren.drawRect(x + 1, y + 1, b.width - 2, b.height - 2, 252, 252, 252);
        } else {
            ren.drawRect(x, y, b.width, b.height, 252, 80, 80);
            ren.drawRect(x + 1, y + 1, b.width - 2, b.height - 2, 252, 180, 180);
        }
    }

    // Draw a particle
    inline void particle(Renderer& ren, const Particle& p, int camX) {
        if (!p.active) return;
        int x = static_cast<int>(p.pos.x) - camX;
        int y = static_cast<int>(p.pos.y);
        float alpha = 1.0f - static_cast<float>(p.life) / p.maxLife;
        int a = static_cast<int>(alpha * 255);
        ren.drawRect(x, y, p.size, p.size, p.r, p.g, p.b, a);
    }

    // Draw level tiles
    inline void level(Renderer& ren, const Level& lvl, int camX) {
        int startCol = camX / TILE_SIZE;
        int endCol = startCol + (GAME_WIDTH / TILE_SIZE) + 2;
        if (endCol > MAP_COLS) endCol = MAP_COLS;

        for (int row = 0; row < MAP_ROWS; row++) {
            for (int col = startCol; col < endCol; col++) {
                int tile = lvl.getTile(col, row);
                if (tile == TILE_EMPTY) continue;

                int x = col * TILE_SIZE - camX;
                int y = row * TILE_SIZE;

                switch (tile) {
                    case TILE_SOLID:
                        ren.drawRect(x, y, TILE_SIZE, TILE_SIZE,
                            lvl.tileColor[0], lvl.tileColor[1], lvl.tileColor[2]);
                        // Top highlight
                        ren.drawRect(x, y, TILE_SIZE, 2,
                            lvl.accentColor[0], lvl.accentColor[1], lvl.accentColor[2]);
                        // Grid lines
                        ren.drawRect(x, y, 1, TILE_SIZE,
                            lvl.tileColor[0] - 20, lvl.tileColor[1] - 20, lvl.tileColor[2] - 20);
                        break;
                    case TILE_SPIKE:
                        // Draw spikes as triangles (approximated with rects)
                        for (int i = 0; i < TILE_SIZE; i += 4) {
                            ren.drawRect(x + i + 1, y + 8, 2, 8, 200, 200, 200);
                            ren.drawRect(x + i, y + 12, 4, 4, 180, 180, 180);
                        }
                        break;
                    case TILE_LADDER:
                        ren.drawRect(x + 2, y, 2, TILE_SIZE, 140, 100, 60);
                        ren.drawRect(x + 12, y, 2, TILE_SIZE, 140, 100, 60);
                        ren.drawRect(x + 2, y + 4, 12, 2, 160, 120, 80);
                        ren.drawRect(x + 2, y + 12, 12, 2, 160, 120, 80);
                        break;
                    case TILE_PLATFORM:
                        ren.drawRect(x, y, TILE_SIZE, 4,
                            lvl.accentColor[0], lvl.accentColor[1], lvl.accentColor[2]);
                        break;
                }
            }
        }
    }

    // Draw HUD
    inline void hud(Renderer& ren, const Player& player, int currentStage) {
        // HP Bar background
        ren.drawRect(8, 16, 8, PLAYER_MAX_HP * 3 + 4, 20, 20, 20);
        ren.drawRectOutline(8, 16, 8, PLAYER_MAX_HP * 3 + 4, 255, 255, 255);

        // HP Bar fill (bottom to top)
        for (int i = 0; i < player.hp; i++) {
            int y = 16 + (PLAYER_MAX_HP - 1 - i) * 3 + 2;
            ren.drawRect(9, y, 6, 2, COL_HP_R, COL_HP_G, COL_HP_B);
        }

        // Lives indicator
        for (int i = 0; i < player.lives; i++) {
            ren.drawRect(24 + i * 10, 16, 6, 6, COL_MEGA_R, COL_MEGA_G, COL_MEGA_B);
        }

        // Stage indicator
        int stgX = GAME_WIDTH - 40;
        ren.drawRect(stgX, 16, 32, 10, 20, 20, 20);
        // Draw "S" + stage number as colored blocks
        ren.drawRect(stgX + 2, 18, 4, 6, 255, 255, 255);
        // Stage number dots
        for (int i = 0; i <= currentStage; i++) {
            ren.drawRect(stgX + 10 + i * 6, 18, 4, 6, 255, 220, 60);
        }
    }

    // Simple text rendering using block letters
    inline void blockChar(Renderer& ren, int x, int y, char c, int r, int g, int b, int scale = 1) {
        // Very basic 5x7 block font for key characters
        // Each character is encoded as 5 columns of 7-bit patterns
        static const unsigned char font[][5] = {
            // A-Z (index 0-25)
            {0x7E,0x09,0x09,0x09,0x7E}, // A
            {0x7F,0x49,0x49,0x49,0x36}, // B
            {0x3E,0x41,0x41,0x41,0x22}, // C
            {0x7F,0x41,0x41,0x41,0x3E}, // D
            {0x7F,0x49,0x49,0x49,0x41}, // E
            {0x7F,0x09,0x09,0x09,0x01}, // F
            {0x3E,0x41,0x49,0x49,0x3A}, // G
            {0x7F,0x08,0x08,0x08,0x7F}, // H
            {0x41,0x41,0x7F,0x41,0x41}, // I
            {0x20,0x40,0x40,0x40,0x3F}, // J
            {0x7F,0x08,0x14,0x22,0x41}, // K
            {0x7F,0x40,0x40,0x40,0x40}, // L
            {0x7F,0x02,0x04,0x02,0x7F}, // M
            {0x7F,0x02,0x04,0x08,0x7F}, // N
            {0x3E,0x41,0x41,0x41,0x3E}, // O
            {0x7F,0x09,0x09,0x09,0x06}, // P
            {0x3E,0x41,0x51,0x21,0x5E}, // Q
            {0x7F,0x09,0x19,0x29,0x46}, // R
            {0x26,0x49,0x49,0x49,0x32}, // S
            {0x01,0x01,0x7F,0x01,0x01}, // T
            {0x3F,0x40,0x40,0x40,0x3F}, // U
            {0x0F,0x30,0x40,0x30,0x0F}, // V
            {0x3F,0x40,0x30,0x40,0x3F}, // W
            {0x63,0x14,0x08,0x14,0x63}, // X
            {0x03,0x04,0x78,0x04,0x03}, // Y
            {0x61,0x51,0x49,0x45,0x43}, // Z
            // 0-9 (index 26-35)
            {0x3E,0x51,0x49,0x45,0x3E}, // 0
            {0x00,0x42,0x7F,0x40,0x00}, // 1
            {0x62,0x51,0x49,0x49,0x46}, // 2
            {0x22,0x41,0x49,0x49,0x36}, // 3
            {0x18,0x14,0x12,0x7F,0x10}, // 4
            {0x27,0x45,0x45,0x45,0x39}, // 5
            {0x3E,0x49,0x49,0x49,0x32}, // 6
            {0x01,0x71,0x09,0x05,0x03}, // 7
            {0x36,0x49,0x49,0x49,0x36}, // 8
            {0x26,0x49,0x49,0x49,0x3E}, // 9
        };

        int idx = -1;
        if (c >= 'A' && c <= 'Z') idx = c - 'A';
        else if (c >= 'a' && c <= 'z') idx = c - 'a';
        else if (c >= '0' && c <= '9') idx = 26 + (c - '0');
        else if (c == '!') { ren.drawRect(x + 2*scale, y, scale, 5*scale, r, g, b); ren.drawRect(x + 2*scale, y + 6*scale, scale, scale, r, g, b); return; }
        else if (c == '-') { ren.drawRect(x + scale, y + 3*scale, 3*scale, scale, r, g, b); return; }
        else return; // Space or unsupported

        if (idx < 0) return;

        for (int col = 0; col < 5; col++) {
            unsigned char bits = font[idx][col];
            for (int row = 0; row < 7; row++) {
                if (bits & (1 << row)) {
                    ren.drawRect(x + col * scale, y + row * scale, scale, scale, r, g, b);
                }
            }
        }
    }

    inline void text(Renderer& ren, int x, int y, const char* str, int r, int g, int b, int scale = 1) {
        int cx = x;
        while (*str) {
            blockChar(ren, cx, y, *str, r, g, b, scale);
            cx += 6 * scale;
            str++;
        }
    }

    inline void textCentered(Renderer& ren, int y, const char* str, int r, int g, int b, int scale = 1) {
        int len = 0;
        const char* s = str;
        while (*s++) len++;
        int w = len * 6 * scale;
        text(ren, (GAME_WIDTH - w) / 2, y, str, r, g, b, scale);
    }
}
