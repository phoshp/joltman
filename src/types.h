#pragma once
#include "raylib.h"
#include <vector>
#include <random>

struct Bullet {
    Vector2 position;
    Vector2 velocity;
    int lifetime;
    int damage;
    Color color;
};

struct WeaponConfig {
    const char* name;
    int damage;
    float spread;
    int pellets;
    int fireDelay;
    float bulletSpeed;
    int maxAmmo;
    int reloadTime;
    Color bulletColor;
};

struct Zombie {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    int health;
    int type;
    float frameTimer;
    int currentFrame;
    int currentAnim;
};

struct ZombieConfig {
    float speed;
    int health;
    float scale;
    Color particleColor;
};

struct Fragment {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotationSpeed;
    int lifetime;
    Color color;
    Vector2 size;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    int lifetime;
    Color color;
};

enum PickupType {
    PICKUP_HEALTH,
    PICKUP_AMMO
};

struct Pickup {
    Vector2 position;
    PickupType type;
    int lifetime;
};

struct Player {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    int health;
    int ammo[3];
    int shootDelay;
    int currentWeapon;
};

enum class GameScreen {
    MENU,
    PLAYING,
    OPTIONS,
    GAME_OVER
};

struct State {
    std::mt19937 rng;

    Texture2D zombieTexs[6];
    Texture2D playerTex;
    Texture2D backgroundTex;
    Texture2D levelBackgrounds[5];

    Sound shootSound;
    Sound reloadSound;
    Sound stepSound;
    Sound zombieSound;
    Sound hitSound;
    Sound dieSound;
    Sound pickupSound;
    Music music;

    float delta;
    int ticks;
    bool gameOver;
    int gameOverTimer;

    GameScreen currentScreen;
    int currentLevel;
    int levelTimer;
    int zombiesToKill;
    int zombiesKilled;

    float damageShakeTimer;
    float damageRedTimer;
    
    float transitionTimer;
    bool isTransitioning;

    std::vector<Particle> particles;
    std::vector<Fragment> fragments;
    std::vector<Zombie> zombies;
    std::vector<Bullet> bullets;
    std::vector<Pickup> pickups;

    Player player;
};
