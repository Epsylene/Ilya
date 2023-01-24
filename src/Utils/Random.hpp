
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
//                return min + (distribution(engine) % (max - min + 1));
                return static_cast<int>(rfloat(min, max));
            }

            static float rfloat(float min = 0.f, float max = 1.f)
            {
                //@todo: replace with an actual, fast PRNG
//                float r = (float)distribution(engine) / (float)std::numeric_limits<uint32_t>::max();
                float r = rand() / (RAND_MAX + 1.0);
                return r * (max - min) + min;
            }

            /// Random vector with component values between `min` and
            /// `max` (in other words, random point in the [min, max]
            /// cube).
            static Vec3 vector(float min = 0.f, float max = 1.f)
            {
                return { rfloat(min, max),
                         rfloat(min, max),
                         rfloat(min, max)};
            }

            /// Random vector that has unit length.
            static Vec3 in_unit_sphere()
            {
                while (true)
                {
                    // A random point inside a unit sphere is a random
                    // vector in a unit cube which has norm less than 1.
                    auto p = Random::vector(-1.f, 1.f);
                    if (square(p) >= 1)
                        continue;

                    return p;
                }
            }

            static Vec3 in_hemisphere(const Vec3& normal)
            {
                auto vec = Random::in_unit_sphere();

                // A random point in the hemisphere is a random point
                // in the upper half of the sphere, as defined by the
                // given normal vector.
                if(dot(vec, normal) > 0.f)
                    return vec;
                else
                    return -vec;
            }

            static Vector3 in_unit_disk()
            {
                // Create random vectors in the [-1, 1] square until the
                // length is inferior to 1 (thus in the unit disk).
                while(true)
                {
                    Vec3 p {rfloat(-1, 1), rfloat(-1, 1), 0.f};
                    if(square(p) >= 1)
                        continue;

                    return p;
                }
            }

            /// Get a random unit vector.
            static Vec3 unit_vector()
            {
                return unit(in_unit_sphere());
            }

            static Vec3 cosine_dir()
            {
                // To generate random directions on a sphere, we only
                // need two random numbers, r1 and r2, because those
                // random directions will be sampled on the surface of
                // the sphere, which is two-dimensional. The first
                // number, r1, is the length (between 0 and 1)
                // corresponding to the angle phi (between 0 and 2*pi):
                // then r1 = phi/(2*pi). The same goes for r2, except
                // that it is the length corresponding to the projection
                // of the altitude length, and that points are
                // distributed on the longitude following a distribution
                // f(theta) (this comes from the supposition that our
                // PDF is rotationaly symmetric around Z, which means
                // that it depends only on theta): r2 =
                // Integral{2*pi*f(u)sin(u)d(u)}. Let's say that
                // f(theta) = cos(theta)/pi, which is the cosine
                // distribution that Lambertian materials follow. Then
                // r2 = 1 - cos^2(theta), and changing to cartesian
                // coordinates we can finally get a random (x, y, z)
                // direction on the sphere that follows this
                // distribution.
                auto r1 = Random::rfloat();
                auto r2 = Random::rfloat();
                auto phi = 2*pi*r1;

                auto x = std::cos(phi)*std::sqrt(r2);
                auto y = std::sin(phi)*std::sqrt(r2);
                auto z = std::sqrt(1 - r2);

                return {x, y, z};
            }

        private:

            static std::mt19937 engine;
            static std::uniform_int_distribution<std::mt19937::result_type> distribution;
    };
}