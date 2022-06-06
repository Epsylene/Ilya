
#pragma once

#include "Ilya.hpp"

struct Vector4
{
    float x, y, z, w;

    constexpr Vector4() = default;

    //////////////////////////////////////////
    /// @brief Construct a Vector4 with all its
    ///     coefficients set to `scalar`
    constexpr explicit Vector4(float scalar): x(scalar), y(scalar),
        z(scalar), w(scalar)
    {}

    /////////////////////////////////////////
    /// @brief Construct a Vector4 from the
    ///     provided scalar coefficients
    constexpr Vector4(float x, float y, float z, float w): x(x), y(y),
       z(z), w(w)
    {}

    constexpr Vector4(const Vector4& vec) = default;
    constexpr Vector4(Vector4&& vec) noexcept = default;
    constexpr Vector4& operator=(const Vector4& vec) = default;

    ///////////////////////////
    /// @brief Unary Vector4 sum
    constexpr Vector4& operator+=(const Vector4& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        w += vec.w;

        return *this;
    }

    //////////////////////////////////
    /// @brief Unary Vector4 difference
    constexpr Vector4& operator-=(const Vector4& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        w -= vec.w;

        return *this;
    }

    //////////////////////////////////////
    /// @brief Unary Vector4 scalar product
    constexpr Vector4& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;

        return *this;
    }

    ///////////////////////////////////////
    /// @brief Unary Vector4 scalar division
    constexpr Vector4& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;

        return *this;
    }

    /////////////////////////////////////////////////////
    /// @brief Unary Vector4 coefficient-wise product
    ///
    /// This is only provided for the sake of convenience,
    /// in dot product functions and the like.
    constexpr Vector4& operator*=(const Vector4& vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;
        w *= vec.w;

        return *this;
    }

    //////////////////////////
    /// @brief Opposite Vector4
    constexpr Vector4 operator-()
    {
        return {-x, -y, -z, -w};
    }

    ///////////////////////////////////
    /// @brief Vector4 equality operator
    bool operator==(const Vector4& rhs) const
    {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z &&
               w == rhs.w;
    }

    /////////////////////////////////////
    /// @brief Vector4 difference operator
    bool operator!=(const Vector4& rhs) const
    {
        return !(rhs == *this);
    }
};

constexpr Vector4 operator+(Vector4 vec1, const Vector4& vec2)
{
    vec1 += vec2;
    return vec1;
}

constexpr Vector4 operator*(Vector4 vec1, float scalar)
{
    vec1 *= scalar;
    return vec1;
}