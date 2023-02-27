
#pragma once

#include "Vector4.hpp"

namespace Ilya
{
    class Matrix4
    {
        private:

            std::array<Vector4, 4> columns {};

        public:

            Matrix4() = default;

            explicit constexpr Matrix4(const Vector4& c1,
               const Vector4& c2, const Vector4& c3, const Vector4& c4):
                    columns({c1, c2, c3, c4}) {}

            constexpr Matrix4(const Matrix4& mat) = default;

            constexpr Vector4& operator[](size_t idx)
            {
                return columns[idx];
            }

            constexpr const Vector4& operator[](size_t idx) const
            {
                return columns[idx];
            }

            constexpr Matrix4& operator+=(const Matrix4& mat)
            {
                for (int i = 0; i < 4; ++i)
                {
                    columns[i] += mat[i];
                }

                return *this;
            }

            constexpr Matrix4& operator-=(const Matrix4& mat)
            {
                for (int i = 0; i < 4; ++i)
                {
                    columns[i] -= mat[i];
                }

                return *this;
            }

            constexpr Matrix4& operator*=(float scalar)
            {
                for (int i = 0; i < 4; ++i)
                {
                    columns[i] *= scalar;
                }

                return *this;
            }

            constexpr Matrix4& operator/=(float scalar)
            {
                auto inv = 1/scalar;
                for (int i = 0; i < 4; ++i)
                {
                    columns[i] *= inv;
                }

                return *this;
            }

            constexpr Matrix4& operator*=(const Matrix4& mat)
            {
                const Vector4 a0 = columns[0], a1 = columns[1], a2 = columns[2], a3 = columns[3];
                const Vector4 b0 = mat[0], b1 = mat[1], b2 = mat[2], b3 = mat[3];

                columns = {a0 * b0[0] + a1 * b0[1] + a2 * b0[2] + a3 * b0[3],
                    a0 * b1[0] + a1 * b1[1] + a2 * b1[2] + a3 * b1[3],
                    a0 * b2[0] + a1 * b2[1] + a2 * b2[2] + a3 * b2[3],
                    a0 * b3[0] + a1 * b3[1] + a2 * b3[2] + a3 * b3[3]};

                return *this;
            }

            constexpr Matrix4 operator-()
            {
                for (int i = 0; i < 4; ++i)
                {
                    columns[i] = -columns[i];
                }

                return *this;
            }

            bool operator==(const Matrix4& rhs) const
            {
                return std::ranges::equal(columns, rhs.columns);
            }

            bool operator!=(const Matrix4& rhs) const
            {
                return !(*this == rhs);
            }
    };

    constexpr Matrix4 operator+(const Matrix4& mat1, const Matrix4& mat2);
    constexpr Matrix4 operator-(const Matrix4& mat1, const Matrix4& mat2);
    constexpr Matrix4 operator*(const Matrix4& mat, float scalar);
    constexpr Matrix4 operator*(const Matrix4& mat1, const Matrix4& mat2);
    constexpr Vector4 operator*(const Matrix4& mat, const Vector4& vec);
}

template<>
struct fmt::formatter<Ilya::Matrix4>
{
constexpr auto parse(format_parse_context& ctx)
{
    return ctx.end();
}

template <typename Context>
auto format(const Ilya::Matrix4& mat, Context& ctx)
{
    return format_to(ctx.out(), "[{} {} {} {}]\n[{} {} {} {}]\n[{} {} {} {}]\n[{} {} {} {}]",
                     mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                     mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                     mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                     mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
}
};