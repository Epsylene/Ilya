
#pragma once

#include "Image.hpp"
#include "Objects/Ray.hpp"
#include "Objects/Hittable.hpp"
#include "Objects/Camera.hpp"

namespace Ilya
{
    class Renderer
    {
        public:

            uint32_t samples_per_pixel, depth;

            Renderer(const Image& img, const HittableList& world, uint32_t samples, uint32_t depth);

            void render(const Camera& cam);

            uint32_t getWidth() const { return img.width; }
            uint32_t getHeight() const { return img.height; }

        private:

            Color ray_color(const Ray& r, const Color& background, int depth);

            Image img;
            HittableList world;
    };
}