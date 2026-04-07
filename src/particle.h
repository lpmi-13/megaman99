#pragma once

#include "vec2.h"
#include <cstdlib>

struct Particle {
    Vec2 pos;
    Vec2 vel;
    int life = 0;
    int maxLife = 30;
    int r = 255, g = 255, b = 255;
    int size = 2;
    bool active = false;

    void update() {
        if (!active) return;
        pos += vel;
        vel.y += 0.1f; // Gravity on particles
        life++;
        if (life >= maxLife) active = false;
    }

    void spawn(float x, float y, int cr, int cg, int cb) {
        pos = {x, y};
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float speed = 1.0f + static_cast<float>(rand() % 30) / 10.0f;
        vel = {cosf(angle) * speed, sinf(angle) * speed - 2.0f};
        r = cr; g = cg; b = cb;
        life = 0;
        maxLife = 15 + rand() % 20;
        size = 1 + rand() % 3;
        active = true;
    }
};
