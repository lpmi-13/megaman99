#pragma once

#include "vec2.h"

struct Rect {
    float x = 0, y = 0, w = 0, h = 0;

    Rect() = default;
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    bool overlaps(const Rect& o) const {
        return x < o.x + o.w && x + w > o.x &&
               y < o.y + o.h && y + h > o.y;
    }

    float right() const { return x + w; }
    float bottom() const { return y + h; }
    float centerX() const { return x + w / 2; }
    float centerY() const { return y + h / 2; }
};
