#pragma once

#include "vec2.h"
#include "rect.h"
#include "bullet.h"
#include "constants.h"
#include <cmath>
#include <cstdlib>

struct Enemy {
    Vec2 pos;
    Vec2 vel;
    int type = ENEMY_MET;
    int hp = 3;
    int maxHp = 3;
    int width = 16;
    int height = 16;
    bool active = false;
    bool facingRight = false;
    int animFrame = 0;
    int animTimer = 0;
    int shootTimer = 0;
    int stateTimer = 0;
    int state = 0; // AI state
    float startX = 0;
    float startY = 0;

    Rect getRect() const {
        return {pos.x, pos.y, static_cast<float>(width), static_cast<float>(height)};
    }

    void spawn(int enemyType, float x, float y) {
        type = enemyType;
        pos = {x, y};
        startX = x;
        startY = y;
        active = true;
        state = 0;
        stateTimer = 0;
        shootTimer = 0;
        vel = {0, 0};

        switch (type) {
            case ENEMY_MET:
                hp = maxHp = 2;
                width = 16; height = 16;
                break;
            case ENEMY_SNIPER:
                hp = maxHp = 5;
                width = 16; height = 24;
                break;
            case ENEMY_FLYING:
                hp = maxHp = 2;
                width = 16; height = 16;
                break;
            case ENEMY_BOSS:
                hp = maxHp = 28;
                width = 32; height = 32;
                break;
        }
    }

    // Returns true if enemy wants to shoot (caller handles bullet creation)
    bool update(float playerX, float playerY) {
        if (!active) return false;

        facingRight = (playerX > pos.x);
        animTimer++;
        if (animTimer >= 8) {
            animTimer = 0;
            animFrame = (animFrame + 1) % 4;
        }
        stateTimer++;
        bool wantsToShoot = false;

        switch (type) {
            case ENEMY_MET:
                updateMet(playerX, playerY, wantsToShoot);
                break;
            case ENEMY_SNIPER:
                updateSniper(playerX, playerY, wantsToShoot);
                break;
            case ENEMY_FLYING:
                updateFlying(playerX, playerY);
                break;
            case ENEMY_BOSS:
                updateBoss(playerX, playerY, wantsToShoot);
                break;
        }

        return wantsToShoot;
    }

    void takeDamage(int dmg) {
        // Mets are invulnerable when hidden (state 0)
        if (type == ENEMY_MET && state == 0) return;

        hp -= dmg;
        if (hp <= 0) {
            active = false;
        }
    }

    float getShootDirX() const { return facingRight ? BULLET_SPEED : -BULLET_SPEED; }
    float getShootDirY() const { return 0; }

private:
    void updateMet(float playerX, float /*playerY*/, bool& wantsToShoot) {
        float dist = fabsf(playerX - pos.x);
        if (state == 0) {
            // Hidden - invulnerable, peek when player is close
            if (dist < 80) {
                state = 1;
                stateTimer = 0;
            }
        } else if (state == 1) {
            // Peeking - vulnerable, shoot after delay
            if (stateTimer > 30) {
                wantsToShoot = true;
                state = 2;
                stateTimer = 0;
            }
        } else {
            // Exposed, go back to hiding
            if (stateTimer > 45) {
                state = 0;
                stateTimer = 0;
            }
        }
    }

    void updateSniper(float playerX, float /*playerY*/, bool& wantsToShoot) {
        // Walk back and forth, shoot periodically
        if (state == 0) {
            vel.x = facingRight ? 0.5f : -0.5f;
            pos.x += vel.x;
            // Reverse at edges
            if (fabsf(pos.x - startX) > 40) {
                state = 1;
                stateTimer = 0;
            }
            shootTimer++;
            if (shootTimer > 90 && fabsf(playerX - pos.x) < 150) {
                wantsToShoot = true;
                shootTimer = 0;
            }
        } else {
            // Pause
            if (stateTimer > 30) {
                vel.x = -vel.x;
                state = 0;
                stateTimer = 0;
            }
        }
    }

    void updateFlying(float playerX, float playerY) {
        // Sine wave flight toward player
        float dx = playerX - pos.x;
        float dy = playerY - pos.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist > 1.0f) {
            pos.x += (dx / dist) * 1.2f;
            pos.y += (dy / dist) * 1.2f;
        }
        pos.y += sinf(stateTimer * 0.08f) * 0.8f;
    }

    void updateBoss(float playerX, float /*playerY*/, bool& wantsToShoot) {
        // Jump around and shoot
        if (state == 0) {
            // Idle
            if (stateTimer > 40) {
                state = 1;
                stateTimer = 0;
                vel.y = -5.0f;
                vel.x = (playerX > pos.x) ? 2.0f : -2.0f;
            }
        } else if (state == 1) {
            // Jumping
            vel.y += GRAVITY;
            pos.x += vel.x;
            pos.y += vel.y;
            if (vel.y > 0 && pos.y >= startY) {
                pos.y = startY;
                vel = {0, 0};
                state = 2;
                stateTimer = 0;
            }
        } else if (state == 2) {
            // Shoot burst
            shootTimer++;
            if (shootTimer % 15 == 0 && shootTimer <= 45) {
                wantsToShoot = true;
            }
            if (stateTimer > 60) {
                state = 0;
                stateTimer = 0;
                shootTimer = 0;
            }
        }
    }
};
