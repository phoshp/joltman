#pragma once
#include "types.h"
#include <random>

inline float randfr(State& state, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(state.rng);
}

inline float randf(State& state) {
    return randfr(state, 0.0f, 1.0f);
}
