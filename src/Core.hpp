
#pragma once

#include "ilpch.hpp"

#define macro_str(a) #a
#define xmacro_str(a) macro_str(a)

namespace fs = std::filesystem;

namespace Ilya
{
    static const std::filesystem::path res_path { xmacro_str(ILYA_RES_DIR) };
    static const std::filesystem::path app_path { xmacro_str(ILYA_APP_DIR) };

    static const float infinity = std::numeric_limits<float>::infinity();
    static const float pi = static_cast<float>(M_PI);

    inline float random_float(float min = 0.f, float max = 1.f)
    {
        float r = rand() / (RAND_MAX + 1.0);
        return min + (max - min)*r;
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