
#pragma once

#include "Ilya.hpp"

#include "Utils/Vector3.hpp"
#include "Objects/Ray.hpp"

namespace Ilya
{
    class Camera
    {
        public:

            float t_open, t_close;
            float aspect, lens;
            Vec3 orig, llc, horizontal, vertical;
            Vec3 u, v, w;

            Camera(Vec3 from, Vec3 at, Vec3 up, float aperture, float focus_dist, float fov, float aspect = 16.f/9.f, float t_open = 0.f, float t_close = 0.f):
                    aspect(aspect), lens(aperture / 2.f), t_open(t_open), t_close(t_close)
            {
                // Our viewport is a plane that ranges [-h,h] in height and
                // [-aspect, aspect] in width; h can then be interpreted as
                // the distance between the field of view cone and the camera
                // -plane axis at the point where the plane is situated, which
                // correspond geometrically to the tangent of half the cone's
                // aperture angle (that is, the FOV angle). The rays are casted
                // from the origin towards this plane and form the image.
                auto h = std::tan(radians(fov)/2.f);
                auto viewport_height = 2.f * h;
                auto viewport_width = viewport_height * aspect;

                // Camera's basis vectors
                w = unit(from - at);
                u = unit(cross(up, w));
                v = cross(w, u);

                orig = from; // the origin point
                horizontal = focus_dist * viewport_width * u;
                vertical = focus_dist * viewport_height * v;
                llc = orig - horizontal/2.f - vertical/2.f - focus_dist*w;
                // the vector going from the origin to the lower left
                // corner of the viewport (note that it is -{0.f, 0.f, focal}
                // because the Z axis points outwards the viewport plane)
            }

            Ray ray(float s, float t) const
            {
                // We take a random vector on the lens border, and use
                // that as an offset in the ray origin to simulate the
                // effect of an actual lens.
                auto rd = lens * rand_in_unit_disk();
                auto offset = u*rd.x + v*rd.y;

                // The "viewport origin" is set at the lower left corner
                // of the viewport plane.
                return {orig + offset, llc + s*horizontal + t*vertical - orig - offset, random_float(t_open, t_close)};
            }
    };
}