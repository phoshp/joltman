#include "raylib.h"
#include "raymath.h"
#include "types.h"
#include "config.h"
#include "utils.h"
#include "game.h"
#include <string>

static void InitLevel(State& state) {
    state.zombies.clear();
    state.bullets.clear();
    state.particles.clear();
    state.pickups.clear();
    state.player.position = {static_cast<float>(SCREEN_W) / 2, static_cast<float>(SCREEN_H) / 2};
    state.player.health = 100;
    state.player.currentWeapon = 0;
    state.player.ammo[0] = WEAPONS[0].maxAmmo;
    state.player.ammo[1] = WEAPONS[1].maxAmmo;
    state.player.ammo[2] = WEAPONS[2].maxAmmo;
    state.player.shootDelay = 0;
    state.zombiesToKill = 20 + state.currentLevel * 15;
    state.zombiesKilled = 0;
    state.levelTimer = 0;
    state.gameOver = false;
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "Joltman - Zombie Shooter");
    InitAudioDevice();
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);

    std::random_device rd;
    State state;
    state.rng = std::mt19937(rd());
    
    state.zombieTexs[0] = LoadTexture("assets/zombies/1ZombieSpriteSheet.png");
    state.zombieTexs[1] = LoadTexture("assets/zombies/2ZombieSpriteSheet.png");
    state.zombieTexs[2] = LoadTexture("assets/zombies/3ZombieSpriteSheet.png");
    state.zombieTexs[3] = LoadTexture("assets/zombies/4ZombieSpriteSheet.png");
    state.zombieTexs[4] = LoadTexture("assets/zombies/5ZombieSpriteSheet.png");
    state.zombieTexs[5] = LoadTexture("assets/zombies/6ZombieSpriteSheet.png");
    
    state.playerTex = LoadTexture("assets/player.png");
    
    state.backgroundTex = LoadTexture("assets/background.png");
    
    // Default load all 5 level backgrounds. We will generate these shortly.
    for (int i = 0; i < 5; i++) {
        std::string bgPath = "assets/level" + std::to_string(i) + "_bg.png";
        Texture2D t = LoadTexture(bgPath.c_str());
        if (t.id != 0) {
            state.levelBackgrounds[i] = t;
        } else {
            state.levelBackgrounds[i] = state.backgroundTex; // fallback
        }
    }

    state.shootSound = LoadSound("assets/sounds/cg1.wav");
    state.reloadSound = LoadSound("assets/sounds/reload.wav");
    state.stepSound = LoadSound("assets/sounds/step.wav");
    state.zombieSound = LoadSound("assets/sounds/zombie.wav");
    state.hitSound = LoadSound("assets/sounds/hit.wav");
    state.dieSound = LoadSound("assets/sounds/die.wav");
    state.pickupSound = LoadSound("assets/sounds/pickup.wav");
    state.music = LoadMusicStream("assets/sounds/music.mp3");

    state.delta = 1.0f / 60.0f;
    state.ticks = 0;
    state.currentScreen = GameScreen::MENU;
    state.currentLevel = 0;
    state.damageShakeTimer = 0.0f;
    state.damageRedTimer = 0.0f;

    PlayMusicStream(state.music);

    while (!WindowShouldClose()) {
        state.ticks++;
        state.delta = GetFrameTime();
        UpdateMusicStream(state.music);

        if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))) {
            ToggleFullscreen();
        }

        float scale = fminf(static_cast<float>(GetScreenWidth())/SCREEN_W, static_cast<float>(GetScreenHeight())/SCREEN_H);
        Vector2 mousePosScreen = GetMousePosition();
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mousePosScreen.x - (GetScreenWidth() - (SCREEN_W*scale))*0.5f)/scale;
        virtualMouse.y = (mousePosScreen.y - (GetScreenHeight() - (SCREEN_H*scale))*0.5f)/scale;
        virtualMouse.x = fmaxf(0.0f, fminf(virtualMouse.x, static_cast<float>(SCREEN_W)));
        virtualMouse.y = fmaxf(0.0f, fminf(virtualMouse.y, static_cast<float>(SCREEN_H)));

        if (state.damageShakeTimer > 0) state.damageShakeTimer -= state.delta;
        if (state.damageRedTimer > 0) state.damageRedTimer -= state.delta;

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (state.currentScreen == GameScreen::MENU) {
            DrawText("JOLTMAN", SCREEN_W / 2 - MeasureText("JOLTMAN", 60)/2, 150, 60, RAYWHITE);
            
            Rectangle playBtn = { SCREEN_W/2.0f - 100, 300, 200, 50 };
            Rectangle optBtn = { SCREEN_W/2.0f - 100, 380, 200, 50 };
            Rectangle exitBtn = { SCREEN_W/2.0f - 100, 460, 200, 50 };

            Vector2 mousePos = virtualMouse;
            bool click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            DrawRectangleRec(playBtn, CheckCollisionPointRec(mousePos, playBtn) ? LIGHTGRAY : DARKGRAY);
            DrawText("PLAY", playBtn.x + 70, playBtn.y + 15, 20, WHITE);
            if (click && CheckCollisionPointRec(mousePos, playBtn)) {
                state.currentScreen = GameScreen::PLAYING;
                state.currentLevel = 0;
                InitLevel(state);
            }

            DrawRectangleRec(optBtn, CheckCollisionPointRec(mousePos, optBtn) ? LIGHTGRAY : DARKGRAY);
            DrawText("OPTIONS", optBtn.x + 50, optBtn.y + 15, 20, WHITE);
            if (click && CheckCollisionPointRec(mousePos, optBtn)) {
                state.currentScreen = GameScreen::OPTIONS;
            }

            DrawRectangleRec(exitBtn, CheckCollisionPointRec(mousePos, exitBtn) ? LIGHTGRAY : DARKGRAY);
            DrawText("EXIT", exitBtn.x + 75, exitBtn.y + 15, 20, WHITE);
            if (click && CheckCollisionPointRec(mousePos, exitBtn)) {
                break;
            }

        } else if (state.currentScreen == GameScreen::OPTIONS) {
            DrawText("OPTIONS", SCREEN_W / 2 - MeasureText("OPTIONS", 40)/2, 200, 40, RAYWHITE);
            DrawText("Use W, A, S, D to move.", SCREEN_W / 2 - 120, 300, 20, GRAY);
            DrawText("Hold SPACE to auto-aim and shoot.", SCREEN_W / 2 - 180, 340, 20, GRAY);
            
            Rectangle backBtn = { SCREEN_W/2.0f - 100, 450, 200, 50 };
            Vector2 mousePos = virtualMouse;
            bool click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
            
            DrawRectangleRec(backBtn, CheckCollisionPointRec(mousePos, backBtn) ? LIGHTGRAY : DARKGRAY);
            DrawText("BACK", backBtn.x + 70, backBtn.y + 15, 20, WHITE);
            if (click && CheckCollisionPointRec(mousePos, backBtn)) {
                state.currentScreen = GameScreen::MENU;
            }

        } else if (state.currentScreen == GameScreen::PLAYING) {
            if (state.player.health <= 0) {
                state.gameOver = true;
            }

            if (state.gameOver) {
                if (IsKeyPressed(KEY_R)) {
                    state.currentScreen = GameScreen::MENU;
                }
            } else {
                state.player.velocity = {0, 0};
                bool moved = false;
                if (IsKeyDown(KEY_W)) { state.player.velocity.y = -PLAYER_SPEED; moved = true; }
                if (IsKeyDown(KEY_S)) { state.player.velocity.y = PLAYER_SPEED; moved = true; }
                if (IsKeyDown(KEY_A)) { state.player.velocity.x = -PLAYER_SPEED; moved = true; }
                if (IsKeyDown(KEY_D)) { state.player.velocity.x = PLAYER_SPEED; moved = true; }

                Vector2 direction = {1.0f, 0.0f};
                float minDist = 999999.0f;
                int nearestIndex = -1;
                for (int i = 0; i < static_cast<int>(state.zombies.size()); i++) {
                    float dist = Vector2Distance(state.player.position, state.zombies[i].position);
                    if (dist < minDist) {
                        minDist = dist;
                        nearestIndex = i;
                    }
                }
                if (nearestIndex != -1) {
                    Vector2 diffPos = Vector2Subtract(state.zombies[nearestIndex].position, state.player.position);
                    direction = Vector2Normalize(diffPos);
                } else if (Vector2LengthSqr(state.player.velocity) > 0.1f) {
                    direction = Vector2Normalize(state.player.velocity);
                } else {
                    float rad = state.player.rotation * (PI / 180.0f);
                    direction = { cosf(rad), sinf(rad) };
                }
                
                if (moved && state.ticks % 20 == 0) {
                    PlaySound(state.stepSound);
                }

                if (IsKeyPressed(KEY_ONE)) state.player.currentWeapon = 0;
                if (IsKeyPressed(KEY_TWO)) state.player.currentWeapon = 1;
                if (IsKeyPressed(KEY_THREE)) state.player.currentWeapon = 2;
                if (IsKeyPressed(KEY_ESCAPE)) state.currentScreen = GameScreen::MENU;

                int cw = state.player.currentWeapon;
                if (state.zombies.size() > 0 && state.player.shootDelay <= 0 && state.player.ammo[cw] > 0) {
                    for (int p = 0; p < WEAPONS[cw].pellets; p++) {
                        float spread = randfr(state, -WEAPONS[cw].spread, WEAPONS[cw].spread);
                        float angle = atan2f(direction.y, direction.x) + spread;
                        Vector2 aimDir = { cosf(angle), sinf(angle) };
                        
                        Bullet b;
                        b.position = Vector2Add(state.player.position, Vector2Scale(aimDir, 60.0f));
                        b.velocity = Vector2Scale(aimDir, WEAPONS[cw].bulletSpeed);
                        b.lifetime = 85;
                        b.damage = WEAPONS[cw].damage;
                        b.color = WEAPONS[cw].bulletColor;
                        state.bullets.push_back(b);
                    }
                    state.player.shootDelay = WEAPONS[cw].fireDelay;
                    state.player.ammo[cw]--;
                    PlaySound(state.shootSound);
                } else if (state.player.shootDelay > 0) {
                    state.player.shootDelay--;
                }

                if (state.player.ammo[cw] <= 0 && state.player.shootDelay <= 0) {
                    state.player.ammo[cw] = WEAPONS[cw].maxAmmo;
                    state.player.shootDelay = WEAPONS[cw].reloadTime;
                    PlaySound(state.reloadSound);
                }

                int spawnRate = std::max(50, 100 - state.currentLevel * 10);
                if (state.ticks % spawnRate == 0 && state.zombies.size() < static_cast<size_t>(15 + state.currentLevel * 5)) {
                    spawnZombie(state);
                }

                if (state.ticks % 300 == 0 && randf(state) < 0.6f) {
                    Pickup p;
                    p.position = { randfr(state, 50, SCREEN_W - 50), randfr(state, 50, SCREEN_H - 50) };
                    p.type = (randf(state) < 0.5f) ? PICKUP_HEALTH : PICKUP_AMMO;
                    p.lifetime = 60 * 10;
                    state.pickups.push_back(p);
                }

                float targetRotation = atan2f(direction.y, direction.x) * (180.0f / PI);
                float diff = targetRotation - state.player.rotation;
                while (diff > 180.0f) diff -= 360.0f;
                while (diff < -180.0f) diff += 360.0f;
                state.player.rotation += diff * 12.0f * state.delta;
                state.player.position = Vector2Add(state.player.position, Vector2Scale(state.player.velocity, state.delta));

                state.player.position.x = fminf(fmaxf(state.player.position.x, 20.0f), static_cast<float>(SCREEN_W - 20));
                state.player.position.y = fminf(fmaxf(state.player.position.y, 20.0f), static_cast<float>(SCREEN_H - 20));

                updateBullets(state);
                updateZombies(state);
                updateParticles(state);
                updateFragments(state);
                updatePickups(state);

                if (state.zombiesKilled >= state.zombiesToKill && !state.isTransitioning) {
                    state.isTransitioning = true;
                    state.transitionTimer = 2.0f;
                }

                if (state.isTransitioning) {
                    state.transitionTimer -= state.delta;
                    // Trigger level switch exactly once when timer crosses 1.0f
                    if (state.transitionTimer <= 1.0f && state.transitionTimer + state.delta > 1.0f) {
                        state.currentLevel++;
                        if (state.currentLevel >= 5) {
                            state.currentLevel = 4;
                            state.zombiesToKill += 20;
                            state.zombiesKilled = 0; // Fixed endless bug
                            state.player.health = 100;
                        } else {
                            InitLevel(state);
                        }
                    }
                    if (state.transitionTimer <= 0.0f) {
                        state.isTransitioning = false;
                    }
                }
            }

            Camera2D camera = {0};
            camera.target = { static_cast<float>(SCREEN_W)/2.0f, static_cast<float>(SCREEN_H)/2.0f };
            camera.offset = { static_cast<float>(SCREEN_W)/2.0f, static_cast<float>(SCREEN_H)/2.0f };
            camera.rotation = 0.0f;
            camera.zoom = 1.0f;

            if (state.damageShakeTimer > 0) {
                camera.offset.x += randfr(state, -15.0f, 15.0f);
                camera.offset.y += randfr(state, -15.0f, 15.0f);
            }

            BeginMode2D(camera);
            
            Texture2D curBg = state.levelBackgrounds[state.currentLevel % 5];
            DrawTexturePro(curBg,
                {0, 0, static_cast<float>(curBg.width), static_cast<float>(curBg.height)},
                {0, 0, static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)},
                {0, 0}, 0.0f, WHITE);

            drawPickups(state);
            drawFragments(state);
            drawZombies(state);

            DrawCircleGradient(state.player.position, 150.0f, Color{255, 255, 200, 100}, Color{255, 255, 200, 0});

            DrawTexturePro(state.playerTex,
                {0, 0, static_cast<float>(state.playerTex.width), static_cast<float>(state.playerTex.height)},
                {state.player.position.x, state.player.position.y,
                 static_cast<float>(state.playerTex.width) * 0.4f,
                 static_cast<float>(state.playerTex.height) * 0.4f},
                {static_cast<float>(state.playerTex.width) * 0.4f / 2.0f,
                 static_cast<float>(state.playerTex.height) * 0.4f / 2.0f},
                state.player.rotation, WHITE);

            drawBullets(state);
            drawParticles(state);
            EndMode2D();

            if (state.isTransitioning) {
                float alpha = 0.0f;
                if (state.transitionTimer > 1.0f) {
                    alpha = 1.0f - (state.transitionTimer - 1.0f); // 0.0 to 1.0
                } else {
                    alpha = state.transitionTimer; // 1.0 to 0.0
                }
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, alpha));
            }

            if (state.damageRedTimer > 0) {
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(RED, state.damageRedTimer * 0.5f)); // fade out
            }

            if (state.gameOver) {
                DrawText("GAME OVER", SCREEN_W / 2 - 100, SCREEN_H / 2 - 30, 40, RED);
                DrawText("Press R to return to MENU", SCREEN_W / 2 - 130, SCREEN_H / 2 + 20, 20, WHITE);
            } else {
                int cw = state.player.currentWeapon;
                DrawText(TextFormat("Level: %d", state.currentLevel + 1), 10, 10, 20, RAYWHITE);
                DrawText(TextFormat("Progress: %d / %d", state.zombiesKilled, state.zombiesToKill), 10, 35, 20, GREEN);
                DrawText(TextFormat("Weapon: %s (Press 1,2,3)", WEAPONS[cw].name), 10, 60, 20, WHITE);
                DrawText(TextFormat("Ammo: %d/%d", state.player.ammo[cw], WEAPONS[cw].maxAmmo), 10, 85, 20, WHITE);
                DrawText(TextFormat("Health: %d", state.player.health), 10, 110, 20, WHITE);
                DrawFPS(SCREEN_W - 100, 10);
            }
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        // Draw render texture to screen, scaled and centered
        DrawTexturePro(target.texture,
            {0.0f, 0.0f, static_cast<float>(target.texture.width), static_cast<float>(-target.texture.height)},
            {(GetScreenWidth() - (static_cast<float>(SCREEN_W)*scale))*0.5f, (GetScreenHeight() - (static_cast<float>(SCREEN_H)*scale))*0.5f, static_cast<float>(SCREEN_W)*scale, static_cast<float>(SCREEN_H)*scale},
            {0.0f, 0.0f}, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(target);

    UnloadSound(state.shootSound);
    UnloadSound(state.reloadSound);
    UnloadSound(state.stepSound);
    UnloadSound(state.zombieSound);
    UnloadSound(state.hitSound);
    UnloadSound(state.dieSound);
    UnloadSound(state.pickupSound);
    UnloadMusicStream(state.music);
    for (int i = 0; i < 6; i++) {
        UnloadTexture(state.zombieTexs[i]);
    }
    UnloadTexture(state.playerTex);
    UnloadTexture(state.backgroundTex);
    for (int i = 0; i < 5; i++) {
        if (state.levelBackgrounds[i].id != state.backgroundTex.id && state.levelBackgrounds[i].id != 0) {
            UnloadTexture(state.levelBackgrounds[i]);
        }
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
