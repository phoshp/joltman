#include "game.h"
#include "config.h"
#include "utils.h"
#include "raymath.h"
#include <algorithm>

void spawnParticles(State& state, Color color, Vector2 position, int amount) {
    for (int i = 0; i < amount; i++) {
        Particle p;
        p.position = position;
        p.velocity = {randfr(state, -250.0f, 250.0f), randfr(state, -250.0f, 250.0f)};
        p.lifetime = 20 + static_cast<int>(randf(state) * 30.0f);
        p.color = color;
        state.particles.push_back(p);
    }
}

void spawnBulletParticles(State& state, Color color, Vector2 position, Vector2 normal, int amount) {
    for (int i = 0; i < amount; i++) {
        Particle p;
        p.position = position;
        float angle = atan2f(normal.y, normal.x) + randfr(state, -0.5f, 0.5f);
        float speed = randfr(state, 100.0f, 400.0f);
        p.velocity = {cosf(angle) * speed, sinf(angle) * speed};
        p.lifetime = 10 + static_cast<int>(randf(state) * 15.0f);
        p.color = color;
        state.particles.push_back(p);
    }
}

void spawnFragments(State& state, Color color, Vector2 position, int amount) {
    for (int i = 0; i < amount; i++) {
        Fragment f;
        f.position = position;
        f.velocity = {randfr(state, -150.0f, 150.0f), randfr(state, -150.0f, 150.0f)};
        f.rotation = randfr(state, 0.0f, 360.0f);
        f.rotationSpeed = randfr(state, -200.0f, 200.0f);
        f.lifetime = 120 + static_cast<int>(randf(state) * 120.0f); // last a few seconds
        f.color = color;
        f.size = {randfr(state, 4.0f, 12.0f), randfr(state, 4.0f, 12.0f)};
        state.fragments.push_back(f);
    }
}

void spawnZombie(State& state) {
    Zombie z;
    int edge = static_cast<int>(randf(state) * 4.0f);
    float x, y;
    switch (edge) {
        case 0: x = randf(state) * SCREEN_W;  y = -40.0f; break;
        case 1: x = SCREEN_W + 40.0f;         y = randf(state) * SCREEN_H; break;
        case 2: x = randf(state) * SCREEN_W;  y = SCREEN_H + 40.0f; break;
        default: x = -40.0f;                  y = randf(state) * SCREEN_H; break;
    }
    z.position = {x, y};
    z.type = static_cast<int>(randf(state) * 6.0f);
    if (z.type > 5) z.type = 5;
    z.health = getZombieHealth(z.type, state.currentLevel);
    z.velocity = {0, 0};
    z.rotation = 0.0f;
    z.frameTimer = 0.0f;
    z.currentFrame = 0;
    z.currentAnim = 0;
    state.zombies.push_back(z);
}

void updateParticles(State& state) {
    for (auto& p : state.particles) {
        p.position = Vector2Add(p.position, Vector2Scale(p.velocity, state.delta));
        p.lifetime--;
    }
    state.particles.erase(
        std::remove_if(state.particles.begin(), state.particles.end(),
            [](const Particle& p) { return p.lifetime <= 0; }),
        state.particles.end());
}

void drawParticles(State& state) {
    for (auto& p : state.particles) {
        float radius = fminf(2.0f * (static_cast<float>(p.lifetime) / 30.0f), 2.0f);
        DrawCircleV(p.position, radius, p.color);
    }
}

void updateFragments(State& state) {
    for (auto& f : state.fragments) {
        f.position = Vector2Add(f.position, Vector2Scale(f.velocity, state.delta));
        f.rotation += f.rotationSpeed * state.delta;
        
        // Add friction so they slide and stop
        f.velocity = Vector2Scale(f.velocity, 0.95f);
        f.rotationSpeed *= 0.95f;
        
        f.lifetime--;
    }
    state.fragments.erase(
        std::remove_if(state.fragments.begin(), state.fragments.end(),
            [](const Fragment& f) { return f.lifetime <= 0; }),
        state.fragments.end());
}

void drawFragments(State& state) {
    for (auto& f : state.fragments) {
        float alpha = fminf(1.0f, static_cast<float>(f.lifetime) / 30.0f);
        Color c = f.color;
        c.a = static_cast<unsigned char>(255 * alpha);
        
        Rectangle dest = {f.position.x, f.position.y, f.size.x, f.size.y};
        Vector2 origin = {f.size.x / 2.0f, f.size.y / 2.0f};
        DrawRectanglePro(dest, origin, f.rotation, c);
    }
}

void updatePickups(State& state) {
    for (int i = static_cast<int>(state.pickups.size()) - 1; i >= 0; i--) {
        state.pickups[i].lifetime--;
        if (state.pickups[i].lifetime <= 0) {
            state.pickups.erase(state.pickups.begin() + i);
            continue;
        }
        float dist = Vector2Distance(state.pickups[i].position, state.player.position);
        if (dist < 30.0f) {
            if (state.pickups[i].type == PICKUP_HEALTH) {
                state.player.health += 30;
                if (state.player.health > 100) state.player.health = 100;
            } else if (state.pickups[i].type == PICKUP_AMMO) {
                for (int w = 0; w < 3; w++) {
                    state.player.ammo[w] += WEAPONS[w].maxAmmo / 2;
                    if (state.player.ammo[w] > WEAPONS[w].maxAmmo) {
                        state.player.ammo[w] = WEAPONS[w].maxAmmo;
                    }
                }
            }
            PlaySound(state.pickupSound);
            state.pickups.erase(state.pickups.begin() + i);
        }
    }
}

void drawPickups(State& state) {
    for (auto& p : state.pickups) {
        Color color = (p.type == PICKUP_HEALTH) ? GREEN : BLUE;
        DrawRectangle(static_cast<int>(p.position.x - 10), static_cast<int>(p.position.y - 10), 20, 20, color);
        DrawText((p.type == PICKUP_HEALTH) ? "H" : "A", static_cast<int>(p.position.x - 5), static_cast<int>(p.position.y - 8), 16, WHITE);
    }
}

void updateBullets(State& state) {
    for (auto& b : state.bullets) {
        b.position = Vector2Add(b.position, Vector2Scale(b.velocity, state.delta));
        b.lifetime--;
    }

    for (int i = static_cast<int>(state.bullets.size()) - 1; i >= 0; i--) {
        if (state.bullets[i].lifetime <= 0) {
            state.bullets.erase(state.bullets.begin() + i);
            continue;
        }
        bool hit = false;
        for (int j = static_cast<int>(state.zombies.size()) - 1; j >= 0; j--) {
            float dist = Vector2Distance(state.bullets[i].position, state.zombies[j].position);
            if (dist < 20.0f) {
                state.zombies[j].health -= state.bullets[i].damage;
                Vector2 bulletNorm = Vector2Normalize(state.bullets[i].velocity);
                spawnBulletParticles(state, state.bullets[i].color, state.bullets[i].position, bulletNorm, 8);
                hit = true;
                PlaySound(state.hitSound);
                if (state.zombies[j].health <= 0) {
                    spawnFragments(state, DARKGRAY, state.zombies[j].position, 5); // Bones/armor
                    spawnFragments(state, MAROON, state.zombies[j].position, 8);  // Flesh/blood
                    spawnParticles(state, ZOMBIE_CONFIGS[state.zombies[j].type].particleColor, state.zombies[j].position, 20);
                    state.zombies.erase(state.zombies.begin() + j);
                    state.zombiesKilled++;
                    PlaySound(state.zombieSound);
                }
                break;
            }
        }
        if (hit) {
            state.bullets.erase(state.bullets.begin() + i);
        }
    }
}

void drawBullets(State& state) {
    for (auto& b : state.bullets) {
        DrawLineEx(b.position, Vector2Add(b.position, Vector2Scale(Vector2Normalize(b.velocity), 10.0f)), 2.0, b.color);
    }
}

void updateZombies(State& state) {
    for (auto& z : state.zombies) {
        Vector2 diff = Vector2Subtract(state.player.position, z.position);
        float dist = Vector2Length(diff);
        
        float speed = getZombieSpeed(z.type, state.currentLevel);

        if (dist > 0) {
            z.velocity = Vector2Scale(Vector2Normalize(diff), speed);
        }
        z.position = Vector2Add(z.position, Vector2Scale(z.velocity, state.delta));

        float targetRotation = atan2f(diff.y, diff.x) * (180.0f / PI);
        float rotDiff = targetRotation - z.rotation;
        while (rotDiff > 180.0f) rotDiff -= 360.0f;
        while (rotDiff < -180.0f) rotDiff += 360.0f;
        z.rotation += rotDiff * 5.0f * state.delta;

        float rotRad = z.rotation * (PI / 180.0f);
        Vector2 dir = {cosf(rotRad), sinf(rotRad)};
        
        int targetAnim = z.currentAnim;
        if (fabs(dir.x) > fabs(dir.y)) {
            if (dir.x > 0) targetAnim = 1; 
            else targetAnim = 3; 
        } else {
            if (dir.y > 0) targetAnim = 0; 
            else targetAnim = 2; 
        }

        if (targetAnim != z.currentAnim) {
            z.currentAnim = targetAnim;
        }

        z.frameTimer += state.delta;
        if (z.frameTimer > 0.15f) {
            z.frameTimer = 0.0f;
            z.currentFrame = (z.currentFrame + 1) % 3;
        }

        if (dist < 25.0f && state.ticks % 30 == 0 && state.player.health > 0) {
            state.player.health -= 15;
            state.damageShakeTimer = 0.3f;
            state.damageRedTimer = 0.3f;
            PlaySound(state.hitSound);
            if (state.player.health <= 0) {
                PlaySound(state.dieSound);
            }
        }
    }
}

void drawZombies(State& state) {
    for (auto& z : state.zombies) {
        Texture2D tex = state.zombieTexs[z.type];
        float scale = ZOMBIE_CONFIGS[z.type].scale;

        float frameWidth = 34.0f;
        float frameHeight = 36.0f;

        int animFrame = z.currentFrame;
        Rectangle sourceRec = {
            static_cast<float>(animFrame) * frameWidth + (11.0f * (float)animFrame),
            static_cast<float>(z.currentAnim) * frameHeight,
            frameWidth,
            frameHeight
        };

        Rectangle destRec = {
            z.position.x,
            z.position.y,
            frameWidth * scale,
            frameHeight * scale
        };

        Vector2 origin = { (frameWidth * scale) / 2.0f, (frameHeight * scale) / 2.0f };

        DrawTexturePro(tex, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}
