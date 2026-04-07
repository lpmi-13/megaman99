#pragma once

#include "vec2.h"
#include "rect.h"
#include "bullet.h"
#include "constants.h"
#include "input.h"
#include "level.h"
#include "audio.h"
#include <cmath>

class Player {
public:
    Vec2 pos;
    Vec2 vel;
    int hp = PLAYER_MAX_HP;
    int maxHp = PLAYER_MAX_HP;
    bool alive = true;
    bool facingRight = true;
    bool onGround = false;
    bool shooting = false;
    int animFrame = 0;
    int animTimer = 0;
    int shootTimer = 0;
    int invincibleTimer = 0;
    int deathTimer = 0;
    int lives = 3;

    Bullet bullets[MAX_BULLETS];

    Rect getRect() const {
        return {pos.x + 3, pos.y, static_cast<float>(PLAYER_WIDTH - 6), static_cast<float>(PLAYER_HEIGHT)};
    }

    void reset(float startX, float startY) {
        pos = {startX, startY};
        vel = {0, 0};
        hp = maxHp;
        alive = true;
        onGround = false;
        invincibleTimer = 0;
        deathTimer = 0;
        shootTimer = 0;
        for (auto& b : bullets) b.active = false;
    }

    void update(const Input& input, const Level& level, Audio& audio) {
        if (!alive) {
            deathTimer++;
            return;
        }

        // Invincibility
        if (invincibleTimer > 0) invincibleTimer--;

        // Horizontal movement
        vel.x = 0;
        if (input.isDown(Input::BTN_LEFT)) {
            vel.x = -PLAYER_SPEED;
            facingRight = false;
        }
        if (input.isDown(Input::BTN_RIGHT)) {
            vel.x = PLAYER_SPEED;
            facingRight = true;
        }

        // Jump
        if (input.isPressed(Input::BTN_JUMP) && onGround) {
            vel.y = PLAYER_JUMP_FORCE;
            onGround = false;
            audio.playJump();
        }
        // Variable jump height
        if (input.isReleased(Input::BTN_JUMP) && vel.y < 0) {
            vel.y *= 0.5f;
        }

        // Gravity
        vel.y += GRAVITY;
        if (vel.y > MAX_FALL_SPEED) vel.y = MAX_FALL_SPEED;

        // Shooting
        if (shootTimer > 0) shootTimer--;
        shooting = false;
        if (input.isPressed(Input::BTN_SHOOT)) {
            shoot(audio);
        }

        // Apply movement with collision
        moveWithCollision(level);

        // Animation
        updateAnimation();

        // Update bullets
        for (auto& b : bullets) {
            if (b.active) {
                b.update();
                // Check bullet vs level collision
                int col = static_cast<int>(b.pos.x + b.width / 2) / TILE_SIZE;
                int row = static_cast<int>(b.pos.y + b.height / 2) / TILE_SIZE;
                if (level.isSolid(col, row)) {
                    b.active = false;
                }
            }
        }

        // Spike check
        Rect r = getRect();
        int col1 = static_cast<int>(r.x) / TILE_SIZE;
        int col2 = static_cast<int>(r.right() - 1) / TILE_SIZE;
        int row2 = static_cast<int>(r.bottom() - 1) / TILE_SIZE;
        if (level.isSpike(col1, row2) || level.isSpike(col2, row2)) {
            die(audio);
        }

        // Fall off screen
        if (pos.y > GAME_HEIGHT + 32) {
            die(audio);
        }
    }

    void takeDamage(int dmg, Audio& audio) {
        if (!alive || invincibleTimer > 0) return;
        hp -= dmg;
        invincibleTimer = INVINCIBILITY_FRAMES;
        audio.playHit();
        // Knockback
        vel.y = -3.0f;
        if (hp <= 0) {
            die(audio);
        }
    }

    void die(Audio& audio) {
        alive = false;
        deathTimer = 0;
        lives--;
        audio.playDeath();
    }

private:
    void shoot(Audio& audio) {
        // Count active bullets
        int activeBullets = 0;
        for (auto& b : bullets) {
            if (b.active) activeBullets++;
        }
        if (activeBullets >= MAX_BULLETS) return;

        // Find inactive bullet slot
        for (auto& b : bullets) {
            if (!b.active) {
                float bx = facingRight ? pos.x + PLAYER_WIDTH : pos.x - 6;
                float by = pos.y + 8;
                float bvx = facingRight ? BULLET_SPEED : -BULLET_SPEED;
                b.spawn(bx, by, bvx, 0, true);
                shooting = true;
                shootTimer = ANIM_SPEED_SHOOT;
                audio.playShoot();
                return;
            }
        }
    }

    void moveWithCollision(const Level& level) {
        // Move X
        pos.x += vel.x;
        Rect r = getRect();

        // Check horizontal collisions
        if (vel.x > 0) {
            int col = static_cast<int>(r.right()) / TILE_SIZE;
            int rowTop = static_cast<int>(r.y) / TILE_SIZE;
            int rowBot = static_cast<int>(r.bottom() - 1) / TILE_SIZE;
            for (int row = rowTop; row <= rowBot; row++) {
                if (level.isSolid(col, row)) {
                    pos.x = col * TILE_SIZE - PLAYER_WIDTH + 3 - 0.01f;
                    vel.x = 0;
                    break;
                }
            }
        } else if (vel.x < 0) {
            int col = static_cast<int>(r.x) / TILE_SIZE;
            int rowTop = static_cast<int>(r.y) / TILE_SIZE;
            int rowBot = static_cast<int>(r.bottom() - 1) / TILE_SIZE;
            for (int row = rowTop; row <= rowBot; row++) {
                if (level.isSolid(col, row)) {
                    pos.x = (col + 1) * TILE_SIZE - 3;
                    vel.x = 0;
                    break;
                }
            }
        }

        // Move Y
        pos.y += vel.y;
        r = getRect();
        onGround = false;

        if (vel.y > 0) {
            // Falling
            int rowBot = static_cast<int>(r.bottom()) / TILE_SIZE;
            int colL = static_cast<int>(r.x) / TILE_SIZE;
            int colR = static_cast<int>(r.right() - 1) / TILE_SIZE;
            for (int col = colL; col <= colR; col++) {
                if (level.isSolid(col, rowBot)) {
                    pos.y = rowBot * TILE_SIZE - PLAYER_HEIGHT;
                    vel.y = 0;
                    onGround = true;
                    break;
                }
            }
        } else if (vel.y < 0) {
            // Rising
            int rowTop = static_cast<int>(r.y) / TILE_SIZE;
            int colL = static_cast<int>(r.x) / TILE_SIZE;
            int colR = static_cast<int>(r.right() - 1) / TILE_SIZE;
            for (int col = colL; col <= colR; col++) {
                if (level.isSolid(col, rowTop)) {
                    pos.y = (rowTop + 1) * TILE_SIZE;
                    vel.y = 0;
                    break;
                }
            }
        }

        // Clamp horizontal position
        if (pos.x < 0) pos.x = 0;
    }

    void updateAnimation() {
        if (!onGround) {
            animFrame = 3; // Jump frame
        } else if (fabsf(vel.x) > 0.1f) {
            animTimer++;
            if (animTimer >= ANIM_SPEED_RUN) {
                animTimer = 0;
                animFrame = (animFrame + 1) % 3;
            }
        } else {
            animFrame = 0; // Idle
            animTimer = 0;
        }
    }
};
