#pragma once
#include "types.h"

constexpr float PLAYER_SPEED = 400.0f;
constexpr float BULLET_SPEED = 800.0f;
constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 720;

const WeaponConfig WEAPONS[] = {
    {"Pistol", 60, 0.05f, 1, 10, 800.0f, 12, 60, GOLD},
    {"Shotgun", 60, 0.25f, 6, 45, 900.0f, 6, 90, RED},
    {"Machine Gun", 50, 0.1f, 1, 4, 1000.0f, 30, 80, ORANGE}
};

const ZombieConfig ZOMBIE_CONFIGS[] = {
    {140.0f, 100, 2.5f, GREEN},  // Type 1
    {190.0f, 80,  2.2f, LIME},   // Type 2
    {100.0f, 250, 3.5f, DARKGREEN}, // Type 3
    {160.0f, 120, 2.8f, GREEN},  // Type 4
    {220.0f, 60,  2.0f, YELLOW}, // Type 5
    {120.0f, 200, 3.2f, PURPLE}  // Type 6
};

// We will scale up these configurations per level
inline float getZombieSpeed(int type, int level) {
    return ZOMBIE_CONFIGS[type].speed * (1.0f + level * 0.1f);
}

inline int getZombieHealth(int type, int level) {
    return static_cast<int>(ZOMBIE_CONFIGS[type].health * (1.0f + level * 0.3f));
}
