
#pragma once

#include <array>
#include <cmath>

struct Vector3
{
    float x, y, z;

    constexpr Vector3() = default;

    //////////////////////////////////////////
    /// @brief Construct a Vector3 with all its
    ///     coefficients set to `scalar`
    constexpr explicit Vector3(float scalar): x(scalar), y(scalar),
        z(scalar)
    {}

    /////////////////////////////////////////
    /// @brief Construct a Vector3 from the
    ///     provided scalar coefficients
    constexpr Vector3(float x, float y, float z): x(x), y(y),
        z(z)
    {}

    constexpr Vector3(const Vector3& vec) = default;
    constexpr Vector3(Vector3&& vec) noexcept = default;
    constexpr Vector3& operator=(const Vector3& vec) = default;

    ///////////////////////////
    /// @brief Unary Vector3 sum
    Vector3& operator+=(const Vector3& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;

        return *this;
    }

    //////////////////////////////////
    /// @brief Unary Vector3 difference
    Vector3& operator-=(const Vector3& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;

        return *this;
    }

    //////////////////////////////////////
    /// @brief Unary Vector3 scalar product
    Vector3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;

        return *this;
    }

    ///////////////////////////////////////
    /// @brief Unary Vector3 scalar division
    Vector3& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;

        return *this;
    }

    /////////////////////////////////////////////////////
    /// @brief Unary Vector3 coefficient-wise product
    ///
    /// This is only provided for the sake of convenience,
    /// in dot product functions and the like.
    Vector3& operator*=(const Vector3& vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;

        return *this;
    }

    //////////////////////////
    /// @brief Opposite Vector3
    Vector3 operator-() const
    {
        return {-x, -y, -z};
    }

    ///////////////////////////////////
    /// @brief Vector3 equality operator
    bool operator==(const Vector3& rhs) const
    {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z;
    }

    /////////////////////////////////////
    /// @brief Vector3 difference operator
    bool operator!=(const Vector3& rhs) const
    {
        return !(rhs == *this);
    }
};

using Vec3 = Vector3;

Vector3 operator+(Vector3 vec1, const Vector3& vec2);
Vector3 operator-(Vector3 vec1, const Vector3& vec2);
Vector3 operator*(Vector3 vec, float scalar);
Vector3 operator*(float scalar, Vector3 vec);
Vector3 operator/(Vector3 vec1, float scalar);

Vector3 unit(const Vector3& vec);