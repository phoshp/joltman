#pragma once
#include "types.h"

void spawnParticles(State& state, Color color, Vector2 position, int amount);
void spawnBulletParticles(State& state, Color color, Vector2 position, Vector2 normal, int amount);
void spawnFragments(State& state, Color color, Vector2 position, int amount);
void spawnZombie(State& state);
void updateParticles(State& state);
void drawParticles(State& state);
void updateFragments(State& state);
void drawFragments(State& state);
void updatePickups(State& state);
void drawPickups(State& state);
void updateBullets(State& state);
void drawBullets(State& state);
void updateZombies(State& state);
void drawZombies(State& state);
