
#pragma once

#include "Utils/Color.hpp"

class Texture
{
    public:

        virtual Color val(float u, float v, const Vec3& p) const = 0;
};

class SolidColor: public Texture
{
    public:

        Color color;

        explicit SolidColor(Color c): color(c) {}
        SolidColor(float r, float g, float b):
            color(r, g, b) {}

        virtual Color val(float u, float v, const Vec3& p) const override
        {
            return color;
        }
};

class CheckerTexture: public Texture
{
    public:

        std::shared_ptr<Texture> even, odd;

        CheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd):
            even(even), odd(odd) {}

        CheckerTexture(Color c1, Color c2): even(std::make_shared<SolidColor>(c1)),
            odd(std::make_shared<SolidColor>(c2)) {}

        virtual Color val(float u, float v, const Vec3& p) const override
        {
            auto sines = std::sin(10*p.x)*std::sin(10*p.y)*std::sin(10*p.z);

            if(sines > 0.f)
                return even->val(u, v, p);
            else
                return odd->val(u, v, p);
        }
};