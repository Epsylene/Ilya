
#include "Matrix4.hpp"

namespace Ilya
{
    constexpr Matrix4 operator+(const Matrix4& mat1, const Matrix4& mat2)
    {
        return Matrix4(mat1[0] + mat2[0], mat1[1] + mat2[1], mat1[2] + mat2[2], mat1[3] + mat2[3]);
    }

    constexpr Matrix4 operator-(const Matrix4& mat1, const Matrix4& mat2)
    {
        return Matrix4(mat1[0] - mat2[0], mat1[1] - mat2[1], mat1[2] - mat2[2], mat1[3] - mat2[3]);
    }

    constexpr Matrix4 operator*(const Matrix4& mat, float scalar)
    {
        return Matrix4(mat[0] * scalar, mat[1] * scalar, mat[2] * scalar, mat[3] * scalar);
    }

    constexpr Matrix4 operator*(const Matrix4& mat1, const Matrix4& mat2)
    {
        const Vector4 a0 = mat1[0], a1 = mat1[1], a2 = mat1[2], a3 = mat1[3];
        const Vector4 b0 = mat2[0], b1 = mat2[1], b2 = mat2[2], b3 = mat2[3];

        return Matrix4 {
                a0 * b0[0] + a1 * b0[1] + a2 * b0[2] + a3 * b0[3],
                a0 * b1[0] + a1 * b1[1] + a2 * b1[2] + a3 * b1[3],
                a0 * b2[0] + a1 * b2[1] + a2 * b2[2] + a3 * b2[3],
                a0 * b3[0] + a1 * b3[1] + a2 * b3[2] + a3 * b3[3] };
    }

    constexpr Vector4 operator*(const Matrix4& mat, const Vector4& vec)
    {
        return { mat[0][0] * vec.x + mat[1][0] * vec.y + mat[2][0] * vec.z + mat[3][0] * vec.w,
               mat[0][1] * vec.x + mat[1][1] * vec.y + mat[2][1] * vec.z + mat[3][1] * vec.w,
               mat[0][2] * vec.x + mat[1][2] * vec.y + mat[2][2] * vec.z + mat[3][2] * vec.w,
               mat[0][3] * vec.x + mat[1][3] * vec.y + mat[2][3] * vec.z + mat[3][3] * vec.w};
    }
}