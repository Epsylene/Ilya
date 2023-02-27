
#pragma once

#include "Core.hpp"

namespace Ilya
{
    struct Vector4
    {
        union
        {
            struct { float x, y, z, w; };
            struct { std::array<float, 4> elemns {}; };
        };

        constexpr Vector4():
            x(0), y(0), z(0), w(0) {}

        constexpr explicit Vector4(float scalar):
            x(scalar), y(scalar), z(scalar), w(scalar) {}

        constexpr Vector4(float x, float y, float z, float w):
            x(x), y(y), z(z), w(w) {}

        constexpr Vector4& operator+=(const Vector4& vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;

            return *this;
        }

        constexpr Vector4& operator-=(const Vector4& vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;

            return *this;
        }

        constexpr Vector4& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;

            return *this;
        }

        constexpr Vector4& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;

            return *this;
        }

        constexpr Vector4& operator*=(const Vector4& vec)
        {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            w *= vec.w;

            return *this;
        }

        constexpr Vector4 operator-() const
        {
            return {-x, -y, -z, -w};
        }

        bool operator==(const Vector4& rhs) const
        {
            return x == rhs.x &&
                   y == rhs.y &&
                   z == rhs.z &&
                   w == rhs.w;
        }

        bool operator!=(const Vector4& rhs) const
        {
            return !(rhs == *this);
        }

        constexpr float& operator[](size_t idx)
        {
            return elemns[idx];
        }

        constexpr const float& operator[](size_t idx) const
        {
            return elemns[idx];
        }
    };

    constexpr Vector4 operator+(Vector4 vec1, const Vector4& vec2)
    {
        vec1 += vec2;
        return vec1;
    }

    constexpr Vector4 operator-(Vector4 vec1, const Vector4& vec2)
    {
        vec1 -= vec2;
        return vec1;
    }

    constexpr Vector4 operator*(Vector4 vec1, float scalar)
    {
        vec1 *= scalar;
        return vec1;
    }
}

template <>
struct fmt::formatter<Ilya::Vector4>
{
constexpr auto parse(format_parse_context& ctx)
{
    return ctx.end();
}

template <typename Context>
auto format(const Ilya::Vector4& vec, Context& ctx)
{
    return format_to(ctx.out(), "({}, {}, {}, {})", vec.x, vec.y, vec.z, vec.w);
}
};