#pragma once

#include "vec2.h"
#include "rect.h"
#include "constants.h"

struct Bullet {
    Vec2 pos;
    Vec2 vel;
    bool active = false;
    bool fromPlayer = true;
    int width = 6;
    int height = 4;
    int damage = 1;
    int lifetime = 0;
    int maxLifetime = 120; // 2 seconds

    Rect getRect() const {
        return {pos.x, pos.y, static_cast<float>(width), static_cast<float>(height)};
    }

    void update() {
        if (!active) return;
        pos += vel;
        lifetime++;
        if (lifetime >= maxLifetime) active = false;
    }

    void spawn(float x, float y, float vx, float vy, bool playerBullet = true) {
        pos = {x, y};
        vel = {vx, vy};
        active = true;
        fromPlayer = playerBullet;
        lifetime = 0;
    }
};
