
#pragma once

#include "Core.hpp"

#include "Utils/Math.hpp"
#include "Utils/Random.hpp"
#include "Objects/Ray.hpp"

namespace Ilya
{
    class Camera
    {
        public:

            Camera(Vec3 from, Vec3 at, Vec3 up, float aperture,
                   float focus_dist, float fov, float aspect = 16.f/9.f,
                   float t_open = 0.f, float t_close = 1.f):
                    aspect(aspect), lens(aperture / 2.f), t_open(t_open), t_close(t_close)
            {
                // Our viewport is a plane that ranges [-h,h] in height
                // and [-aspect, aspect] in width; h can then be
                // interpreted as the distance between the field of view
                // cone and the camera -plane axis at the point where
                // the plane is situated, which correspond geometrically
                // to the tangent of half the cone's aperture angle
                // (that is, the FOV angle). The rays are cast from the
                // origin towards this plane and form the image.
                auto h = std::tan(radians(fov)/2.f);
                auto viewport_height = 2.f * h;
                auto viewport_width = viewport_height * aspect;

                // Camera basis vectors
                w = normalize(from - at);
                u = normalize(cross(up, w));
                v = cross(w, u);

                orig = from; // the origin point
                horizontal = focus_dist * viewport_width * u;
                vertical = focus_dist * viewport_height * v;
                llc = orig - horizontal/2.f - vertical/2.f - focus_dist*w;
                // The vector going from the origin to the lower left
                // corner of the viewport (note that it is -{0.f, 0.f,
                // focal} because the Z axis points outwards the
                // viewport plane).
            }

            Ray ray(float s, float t) const
            {
                // We take a random vector on the lens border, and use
                // that as an offset in the ray origin to simulate the
                // effect of an actual lens.
                auto rdv = lens * Random::in_unit_disk();
                auto offset = u*rdv.x + v*rdv.y;

                // The "viewport origin" is set in the lower left corner
                // (llc) of the viewport plane.
                return {orig + offset, llc + s*horizontal + t*vertical - orig - offset,
                        Random::rfloat(t_open, t_close)};
            }

        public:

            float t_open, t_close;
            float aspect, lens;
            Vec3 orig, llc, horizontal, vertical;
            Vec3 u, v, w;
    };
}