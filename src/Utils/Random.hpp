
#pragma once

#include "Core.hpp"
#include "Vector3.hpp"

namespace Ilya
{
    class Random
    {
        public:

            static uint32_t uint()
            {
                return distribution(engine);
            }

            static uint32_t uint(uint32_t min, uint32_t max)
            {
                return min + (distribution(engine) % (max - min + 1));
            }

            static float rfloat(float min = 0.f, float max = 1.f)
            {
                float r = (float)distribution(engine) / (float)std::numeric_limits<uint32_t>::max();
                return r * (max - min) + min;
            }

            static Vec3 vec3(float min = 0.f, float max = 1.f)
            {
                return {rfloat(min, max), rfloat(min, max), rfloat(min, max)};
            }

            static Vec3 in_unit_sphere()
            {
                while (true)
                {
                    // A random point inside a unit sphere is a random
                    // vector in a unit cube which has norm less than 1.
                    auto p = vec3(-1.f, 1.f);
                    if (square(p) >= 1)
                        continue;

                    return p;
                }
            }

            static Vector3 in_unit_disk()
            {
                while(true)
                {
                    Vec3 p {random_float(-1, 1), random_float(-1, 1), 0.f};
                    if(square(p) >= 1)
                        continue;

                    return p;
                }
            }

            static Vec3 unit_vector()
            {
                return unit(in_unit_sphere());
            }

        private:

            static std::mt19937 engine;
            static std::uniform_int_distribution<std::mt19937::result_type> distribution;
    };
}