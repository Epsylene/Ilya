
#pragma once

#include "Utils/Color.hpp"
#include "Utils/Perlin.hpp"

#include <stb_image.h>

namespace Ilya
{
    class Texture
    {
        public:

            virtual Color val(float u, float v, const Point3& p) const = 0;
    };

    /// Solid (uniform) color texture
    class SolidColor: public Texture
    {
        public:

            explicit SolidColor(const Color& c): color(c) {}
            SolidColor(float r, float g, float b):
                    color(r, g, b) {}

            Color val(float u, float v, const Point3& p) const override
            {
                return color;
            }

        public:

            Color color;
    };

    /// Checker texture of two subtextures
    class CheckerTexture: public Texture
    {
        public:

            CheckerTexture(const Ref<Texture>& even, const Ref<Texture>& odd):
                    even(even), odd(odd) {}

            CheckerTexture(Color c1, Color c2): even(std::make_shared<SolidColor>(c1)),
                                                odd(std::make_shared<SolidColor>(c2)) {}

            Color val(float u, float v, const Point3& p) const override
            {
                auto sines = std::sin(10*p.x)*std::sin(10*p.y)*std::sin(10*p.z);

                if(sines > 0.f)
                    return even->val(u, v, p);
                else
                    return odd->val(u, v, p);
            }

        public:

            Ref<Texture> even, odd;
    };

    /// Perlin noise texture
    class NoiseTexture: public Texture
    {
        public:

            explicit NoiseTexture(float scale): scale(scale) {}

            Color val(float u, float v, const Point3& p) const override
            {
                return 0.5f * Color{1, 1, 1} * (1 + std::sin(scale*p.z + 10*perlin.turbulence(p)));
            }

        public:

            Perlin perlin;
            float scale;
    };

    /// Texture that is an image
    class ImageTexture: public Texture
    {
        public:

            explicit ImageTexture(const std::string& path)
            {
                int components_per_pixel;

                auto fullpath = res_path.string() + "/" + path;
                data = stbi_load(fullpath.c_str(), &width, &height, &components_per_pixel, bytes_per_pixel);
                if(!data)
                {
                    error("ERROR: could not load texture image file at path {}\n", fullpath);
                }

                bytes_per_scanline = bytes_per_pixel * width;
            }

            Color val(float u, float v, const Point3& p) const override
            {
                if(!data) return {};

                // Clamp (u, v) to the texture coordinate range
                // [0,1]x[1,0].
                u = std::clamp(u, 0.f, 1.f);
                v = 1.f - std::clamp(v, 0.f, 1.f);

                // Then multiply by width and height to get the
                // actual image coordinates.
                auto i = static_cast<int>(u * width);
                auto j = static_cast<int>(v * height);

                if(i >= width) i = width - 1;
                if(j >= height) j = height - 1;

                // Pixel info is packed as (RGB)_1(RGB)_2(RGB)_3...
                // (that is, groups of 3 bytes each corresponding
                // to a pixel; 'data' is the origin of our array,
                // so moving i*bytes_per_pixel and j*bytes_per_scanline
                // give us the (i, j) pixel index in the array. Then
                // acessing pixel[0, 1, 2] give us the R, G, B values
                // (in the [0,255] range, thus the normalization).
                auto pixel = data + i*bytes_per_pixel + j*bytes_per_scanline;
                return Color(pixel[0], pixel[1], pixel[2]) / 255.f;
            }

        public:

            unsigned char* data;
            int width, height, bytes_per_scanline;
            const static int bytes_per_pixel = 3;
    };
}