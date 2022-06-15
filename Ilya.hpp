
#pragma once

#include <cmath>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <algorithm>

const float infinity = std::numeric_limits<float>::infinity();
const float pi = static_cast<float>(M_PI);

inline float random_float(float min = 0.f, float max = 1.f)
{
    static std::uniform_real_distribution<float> distr(min, max);
    static std::mt19937 gen;

    return distr(gen);
}

inline int random_int(int min, int max)
{
    return static_cast<int>(random_float(min, max));
}

inline float radians(float degrees)
{
    return degrees * pi / 180.f;
}