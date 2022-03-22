#pragma once

#include <random>  // for std::mt19937

class Random {
private:
    static uint_fast32_t seed;
    static std::mt19937 mersenne;

public:
    ///@return Return the seed bound to the RNG
    static uint_fast32_t GetSeed();

    /**
     * Example values are:
     * std::chrono::high_resolution_clock::now().time_since_epoch().count()
     * static_cast<std::mt19937::result_type>(std::time(nullptr)) -> needs <ctime>
     * std::mt19937::default_seed
     */
    static void SetSeed(uint_fast32_t seed);

    ///@return Return a random int in the range of [min, max]
    static int Range(int min, int max);

    ///@return Return a random float in the range of [min, max]
    static float Range(float min, float max);

    ///@return Return a random number between [0, 1]
    static float Next();
};