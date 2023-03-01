
#pragma once

#include "Math.hpp"

namespace Ilya
{
    enum class Axis
    {
        X = 0, Y = 1, Z = 2
    };

    /// @brief Homogeneous transformation class
    ///
    /// Transformation as described by a 4x4 matrix
    /// in homogeneous coordinates. Contains inverse
    /// as well to speed up calculations.
    class Transform
    {
        public:

            Transform(const Mat4& transform, const Mat4& inverse):
                transform(transform), inv(inverse) {}

            explicit Transform(const Mat4& transform):
                Transform(transform, inverse(transform))
            {}

            Vec3 operator()(const Vec3& v) const;
            Point3 operator()(const Point3& p) const;
            Normal operator()(const Normal& n) const;

        private:

            Mat4 transform;
            Mat4 inv;
    };

    Transform translate(const Vec3& delta);
    Transform scale(const Vec3& factor);
    template<Axis axis> Transform rotate(float angle);
    Transform rotate(float angle, const Vec3& axis);

    Transform look_at(const Vec3& pos, const Vec3& at, const Vec3& up);
}