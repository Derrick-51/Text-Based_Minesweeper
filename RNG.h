#ifndef RNG_H
#define RNG_H

#include <random>

extern std::mt19937 mt;
extern std::uniform_int_distribution<int> randPercent;

int generateRandInt(int min, int max);

#endif