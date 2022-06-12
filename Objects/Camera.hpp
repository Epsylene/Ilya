
#pragma once

#include "Ilya.hpp"

#include "Utils/Vector3.hpp"
#include "Objects/Ray.hpp"

class Camera
{
    public:

        float aspect;
        Vec3 orig, llc, horizontal, vertical;

        Camera(Vec3 from, Vec3 at, Vec3 up, float fov, float aspect = 16.f/9.f): aspect(aspect)
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
            auto w = unit(from - at);
            auto u = unit(cross(up, w));
            auto v = cross(w, u);

            orig = from; // the origin point
            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            llc = orig - horizontal/2.f - vertical/2.f - w;
                // the vector going from the origin to the lower left
                // corner of the viewport (note that it is -{0.f, 0.f, focal}
                // because the Z axis points outwards the viewport plane)
        }

        Ray ray(float u, float v) const
        {
            // The "viewport origin" is set at the lower left corner
            // of the viewport plane.
            return {orig, llc + u*horizontal + v*vertical - orig};
        }
};
