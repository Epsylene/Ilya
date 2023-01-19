
#pragma once

#include "ilpch.hpp"

#define macro_str(a) #a
#define xmacro_str(a) macro_str(a)

namespace fs = std::filesystem;

namespace Ilya
{
    template<typename T> using Ref = std::shared_ptr<T>;

    static const fs::path res_path { xmacro_str(ILYA_RES_DIR) };
    static const fs::path app_path { xmacro_str(ILYA_APP_DIR) };

    static const float infinity = std::numeric_limits<float>::infinity();
    static const float pi = static_cast<float>(M_PI);
    static const float epsilon = std::numeric_limits<float>::epsilon();

    template<typename... T>
    inline void print(fmt::format_string<T...> msg, T&&... args)
    {
        fmt::print(msg, args...);
    }

    template<typename... T>
    inline void error(std::string_view msg, T&&... args)
    {
        fmt::print(fmt::fg(fmt::color::red), msg, args...);
    }

    inline float radians(float degrees)
    {
        return degrees * pi / 180.f;
    }
}