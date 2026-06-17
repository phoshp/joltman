#include "raylib.h"
#include "raymath.h"
#include <cstdlib>
#include <limits.h>
#include <math.h>
#include <random>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


struct Bullet {
    Vector2 position;
    Vector2 velocity = {0, 0};
    int lifetime;
};

struct Zombie {
    Vector2 position;
    Vector2 velocity = {0, 0};
    float rotation = 0.0f;
    int health = 100;
};

enum ParticleType {
    PARTICLE_BLOOD,
    PARTICLE_SLIME,
    PARTICLE_FIRE,
    PARTICLE_SPARK,
    PARTICLE_COUNT
};

struct Particle {
    Vector2 position;
    Vector2 velocity = {0, 0};
    int lifetime;
    int index;
    enum ParticleType type;
};

struct Player {
    Vector2 position;
    Vector2 velocity = {0, 0};
    float rotation = 0.0f;
    int health = 100;
    int ammo = 60;
    int shootDelay = 0;
};

struct State {
    std::mt19937 rng;

    Texture2D zombieTex;
    Texture2D playerTex;
    Texture2D backgroundTex;
    Sound shootSound;
    Sound reloadSound;
    Music music;

    float delta;
    int ticks;

    std::vector<Particle> particles;
    std::vector<Zombie> zombies;
    std::vector<Bullet> bullets;

    Player player;
};

float randfr(State& state, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(state.rng);
}

float randf(State& state) {
    return randfr(state, 0.0, 1.0);
}

void drawZombie(State& state, Zombie& zombie) {
    DrawTextureEx(state.zombieTex, zombie.position, zombie.rotation, 1.0, WHITE);
}

void drawParticle(State& state, Particle& particle) {
    Color color;
    switch (particle.type) {
        case PARTICLE_BLOOD:
            color = RED;
            break;
        case PARTICLE_SLIME:
            color = GREEN;
            break;
        case PARTICLE_FIRE:
            color = ORANGE;
            break;
        case PARTICLE_SPARK:
            color = YELLOW;
            break;
        default:
            color = WHITE;
            break;
    }
    particle.position = Vector2Add(particle.position, Vector2Scale(particle.velocity, (1/60.0f)));
    DrawCircleV(particle.position, fminf(2.0 * ((float)particle.lifetime / 100.0), 2.0), color);

    particle.lifetime--;
    if (particle.lifetime <= 0) {
        state.particles.erase(state.particles.begin() + particle.index);
    }
}


void spawnParticle(struct Particle particle, int amount) {
    for (int i = 0; i < amount; i++) {
        particle.velocity.x += 850.0f * std::rand * 2.0f - 1.0f;
        particle.velocity.y += 850.0f * randf() * 2.0f - 1.0f;
        size_t i = arrpush(particles, particle);
    }
}
const float PLAYER_SPEED = 400.0f;
const float BULLET_SPEED = 800.0f;

int main(void) {
    InitWindow(1024, 720, "Joltman Game");
    SetTargetFPS(60);
    InitAudioDevice();
    SetMasterVolume(0.5f);

    std::random_device rd;
    State state = {
        .rng = std::mt19937(rd()),

        .zombieTex = LoadTexture("assets/zombies/4ZombieFrontSPAWN.png"),
        .playerTex = LoadTexture("assets/player.png"),
        .backgroundTex = LoadTexture("assets/background.png"),
        .shootSound = LoadSound("assets/sounds/cg1.wav"),
        .reloadSound = LoadSound("assets/sounds/reload.wav"),
        .music = LoadMusicStream("assets/sounds/music.mp3"),

        .delta = 1.0f / 60.0f,
        .ticks = 0,
        .particles = {},
        .zombies = {},
        .bullets = {},
        .player = {
            .position = (Vector2){512, 360},
        }
    };

    while (!WindowShouldClose()) {
        state.ticks++;
        UpdateMusicStream(state.music);

        Vector2 mousePos = GetMousePosition();
        Vector2 diffPos = Vector2Subtract(mousePos, state.player.position);
        Vector2 direction = Vector2Normalize(diffPos);

        if (IsKeyDown(KEY_W)) {
            state.player.velocity.y = -PLAYER_SPEED;
        } else if (IsKeyDown(KEY_S)) {
            state.player.velocity.y = PLAYER_SPEED;
        }
        if (IsKeyDown(KEY_A)) {
            state.player.velocity.x = -PLAYER_SPEED;
        } else if (IsKeyDown(KEY_D)) {
            state.player.velocity.x = PLAYER_SPEED;
        }
        if (IsKeyDown(KEY_SPACE) && state.player.shootDelay <= 0 && state.player.ammo > 0) {
            state.bullets.push_back({
                .position = Vector2Add(state.player.position, Vector2Scale(direction, 60.0f)),
                .velocity = Vector2Scale(direction, BULLET_SPEED),
                .lifetime = 85
            });
            state.player.shootDelay = 5;
            state.player.ammo--;
            PlaySound(state.shootSound);
        } else {
            state.player.shootDelay--;
        }
        if (state.player.ammo == 0) {
            state.player.ammo = 60;
            state.player.shootDelay = 80;
            PlaySound(state.reloadSound);
        }


        if (state.ticks % 80 == 0) {
            state.zombies.push_back({
                .position = (Vector2){randf(&state) * 800.0f, randf(&state) * 600.0f},
            });
        }

        state.player.rotation = atan2f(direction.y, direction.x) * (180.0f / PI);
        state.player.position = Vector2Add(state.player.position, Vector2Scale(state.player.velocity, state.delta));
        state.player.velocity = Vector2Scale(state.player.velocity, 0.6f);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexturePro(state.backgroundTex, (Rectangle){0, 0, state.backgroundTex.width, state.backgroundTex.height}, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexturePro(state.playerTex,
                {0, 0, (float)state.playerTex.width, (float)state.playerTex.height},
                {state.player.position.x,state.player.position.y, (float)state.playerTex.width * 0.4f, (float)state.playerTex.height * 0.4f},
                {((float)state.playerTex.width * 0.4f) / 2.0f, ((float)state.playerTex.height * 0.4f) / 2.0f},
                state.player.rotation, WHITE);

        for (int i = bulletIndex; i > 0; i--) {
            if (bullets[i].lifetime <= 0) continue;
            bullets[i].position = Vector2Add(bullets[i].position, Vector2Scale(bullets[i].velocity, dt));
            DrawLineEx(bullets[i].position, Vector2Add(bullets[i].position, Vector2Scale(bullets[i].velocity, dt)), 2.0, GOLD);
            bullets[i].lifetime--;
            if (bullets[i].lifetime <= 0) {
                bulletIndex = MAX(bulletIndex, i);
            }
        }

        for (int i = 0; i < zombieCount; i++) {
            drawZombie(&zombies[i]);
        }

        DrawText(TextFormat("Ammo: %d/60", ammo), 10, 40, 20, WHITE);
        DrawFPS(10, 10);
        EndDrawing();


    }
    UnloadSound(shootSound);
    UnloadSound(reloadSound);
    UnloadMusicStream(music);
    UnloadTexture(backgroundTex);
    UnloadTexture(playerTex);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
