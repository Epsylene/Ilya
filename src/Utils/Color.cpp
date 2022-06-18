
#include "Color.hpp"

namespace Ilya
{

    const Color Color::Black {0.f, 0.f, 0.f};
    const Color Color::White {1.f, 1.f, 1.f};
    const Color Color::Red {1.f, 0.f, 0.f};
    const Color Color::Green {0.f, 1.f, 0.f};
    const Color Color::Blue {0.f, 0.f, 1.f};
    const Color Color::Yellow {0.f, 1.f, 1.f};
    const Color Color::Magenta {1.f, 0.f, 1.f};
    const Color Color::Brown {1.f, 1.f, 0.f};

    Color::Color(float factor): r(factor), g(factor), b(factor), a(1.f)
    {}

    Color::Color(float r, float g, float b, float a): r(r), g(g),
                                                      b(b), a(a)
    {}

    Color::Color(const Vector3& color): r(color.x), g(color.y),
                                        b(color.z), a(1.f)
    {}

    Color::Color(const Vector4& color): r(color.x), g(color.y),
                                        b(color.z), a(color.w)
    {}

    Color& Color::operator+=(const Color& color)
    {
        r += color.r;
        g += color.g;
        b += color.b;
        a += color.a;

        return *this;
    }

    Color& Color::operator-=(const Color& color)
    {
        r -= color.r;
        g -= color.g;
        b -= color.b;
        a -= color.a;

        return *this;
    }

    Color& Color::operator*=(const Color& color)
    {
        r *= color.r;
        g *= color.g;
        b *= color.b;
        a *= color.a;

        return *this;
    }

    Color& Color::operator*=(float factor)
    {
        r *= factor;
        g *= factor;
        b *= factor;
        a *= factor;

        return *this;
    }

    Color::operator Vector3() const
    {
        return Vector3(r, g, b);
    }

    Color::operator Vector4() const
    {
        return Vector4(r, g, b, a);
    }

    Color operator+(const Color& c1, const Color& c2)
    {
        return { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a };
    }

    Color operator-(const Color& c1, const Color& c2)
    {
        return { c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a };
    }

    Color operator*(const Color& c1, const Color& c2)
    {
        return { c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a };
    }

    Color operator*(const Color& color, float factor)
    {
        return { color.r * factor, color.g * factor, color.b * factor, color.a };
    }

    Color operator*(float factor, const Color& color)
    {
        return { color.r * factor, color.g * factor, color.b * factor, color.a };
    }
}