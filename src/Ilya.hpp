
#pragma once

#include <cmath>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <algorithm>
#include <filesystem>

#define macro_str(a) #a
#define xmacro_str(a) macro_str(a)

namespace Ilya
{
    static const std::filesystem::path res_path { xmacro_str(ILYA_RES_DIR) };

    static const float infinity = std::numeric_limits<float>::infinity();
    static const float pi = static_cast<float>(M_PI);

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
}