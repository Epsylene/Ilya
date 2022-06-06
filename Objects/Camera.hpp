
#pragma once

#include "Ilya.hpp"

#include "Utils/Vector3.hpp"
#include "Objects/Ray.hpp"

class Camera
{
    public:

        float aspect = 16.f/9.f, focal = 1.f;
        Vec3 orig, llc, horizontal, vertical;

        Camera()
        {
            // Our viewport is a plane that ranges [-1,1] in height and
            // [-aspect, aspect] in width. The rays all come from the
            // origin and go towards this plane; the distance between the
            // origin and the plane is called the "focal length" (here
            // set at 1).
            auto viewport_height = 2.f;
            auto viewport_width = viewport_height*aspect;

            orig = {}; // the origin point
            horizontal = {viewport_width, 0.f, 0.f};
            vertical = {0.f, viewport_height, 0.f};
            llc = orig - horizontal/2.f - vertical/2.f - Vec3(0.f, 0.f, focal);
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
