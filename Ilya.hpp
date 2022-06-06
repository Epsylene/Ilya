
#pragma once

#include <cmath>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

const float infinity = std::numeric_limits<float>::infinity();

inline float random_float(float min = 0.f, float max = 1.f)
{
    static std::uniform_real_distribution<float> distr(min, max);
    static std::mt19937 gen;

    return distr(gen);
}