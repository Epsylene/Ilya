
#pragma once

#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Ilya
{
    class Color
    {
        public:

            float r, g, b, a;

            static const Color Black;
            static const Color White;
            static const Color Red;
            static const Color Green;
            static const Color Blue;
            static const Color Yellow;
            static const Color Magenta;
            static const Color Brown;

            Color() = default;

            explicit Color(float factor);

            ///////////////////////////////////////////////
            /// @brief Constructs an RGBA color
            ///
            /// The parameters are expected to range from
            /// 0 to 1, with the alpha set to 1 by default.
            Color(float r, float g, float b, float a = 1.f);

            /////////////////////////////////
            /// @brief Construct an RGB color
            Color(const Vector3& color);

            /////////////////////////////////
            /// @brief Construct an RGBA color
            Color(const Vector4& color);

            Color& operator+=(const Color& color);
            Color& operator-=(const Color& color);
            Color& operator*=(const Color& color);
            Color& operator*=(float factor);

            operator Vector3() const;
            operator Vector4() const;
    };

    Color operator+(const Color& c1, const Color& c2);
    Color operator-(const Color& c1, const Color& c2);
    Color operator*(const Color& c1, const Color& c2);
    Color operator*(const Color& color, float factor);
    Color operator*(float factor, const Color& color);
    Color operator/(const Color& color, float factor);
}