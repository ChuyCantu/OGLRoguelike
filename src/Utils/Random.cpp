#include "Random.hpp"

uint_fast32_t Random::seed;  
std::mt19937 Random::mersenne;

uint_fast32_t Random::GetSeed() {
    return seed;
}

void Random::SetSeed(uint_fast32_t seed) {
    mersenne.seed(seed);
    Random::seed = seed;
}

int Random::Range(int min, int max) {
    std::uniform_int_distribution die{min, max};
    return die(mersenne);
}

float Random::Range(float min, float max) {
    std::uniform_real_distribution die{min, max};
    return die(mersenne);
}

float Random::Next() {
    std::uniform_real_distribution die{0.f, 1.f};
    return die(mersenne);
}