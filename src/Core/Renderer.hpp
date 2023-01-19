
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

            /// Render the image producing a number of rays per pixel from
            /// the camera in a random direction and calling `ray_color()`
            /// to get the pixel color.
            void render(const Camera& cam, const Ref<Hittable>& light);

            uint32_t getWidth() const { return img.width; }
            uint32_t getHeight() const { return img.height; }

        private:

            /// Take a ray `r` and recursively hit while it is not absorbed
            /// with depth `depth`. If it doesn't hit anything, return
            /// `background`; else, the ray color and emission light.
            Color ray_color(const Ray& r, const Ref<Hittable>& light,
                            const Color& background, int depth);

            Image img;
            HittableList world;
    };
}