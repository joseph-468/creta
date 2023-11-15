#pragma once

#define BLACK 0x000000
#define GRAY 0x444444
#define WHITE 0xFFFFFF
#define RED 0x00FF00
#define GREEN 0x00FF00
#define BLUE 0x00FF00

inline int clamp(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

