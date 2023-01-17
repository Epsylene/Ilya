
#pragma once

#include "Core.hpp"
#include "Vector3.hpp"
#include "Objects/Hittable.hpp"

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
                    auto p = Random::vec3(-1.f, 1.f);
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
                    Vec3 p {random_float(-1, 1), random_float(-1, 1), 0.f};
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
                // integral{2*pi*f(u)sin(u)d(u)}. Let's say that
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

    /// A PDF (Probability Density Function) is a probability
    /// distribution of points in space; that is, it is a function that
    /// gives the probability for a given vector to be randomly
    /// generated. The PDF class does at the same time return this
    /// probability for any given vector, and work as a random vector
    /// generator following a distribution.
    class PDF
    {
        public:

            /// Sends a random vector following the PDF distribution.
            virtual Vec3 random_vector() const = 0;

            /// Gives the value of the PDF at the point `dir`.
            virtual float val(const Vec3& dir) const = 0;
    };

    /// Cosine distribution PDF, used for example in Lambertian
    /// materials.
    class CosinePDF: public PDF
    {
        public:

            explicit CosinePDF(const Vec3& w): uvw(w) {}

            virtual Vec3 random_vector() const override
            {
                // Generate a random vector following a cosine
                // distribution in the local basis:
                return uvw.local(Random::cosine_dir());
            }

            virtual float val(const Vec3& dir) const override
            {
                // If the cosine is nonpositive, that is, if the angle
                // between the random vector and the surface normal is
                // greater than pi/2 (which means that the random vector
                // is directed towards the surface), return 0; in the
                // other case, return the cosine (given by Lambert's
                // cosine law) divided by pi, which is the value of the
                // probability for the cosine distribution over [0, pi].
                auto cos = dot(unit(dir), uvw.w);
                return (cos <= 0) ? 0 : cos/pi;
            }

        public:

            ONB uvw;
    };

    /// Hittable-oriented distribution, that is, the probability
    /// distribution of random vectors on the surface of a given
    /// hittable. This is useful for example to do importance sampling of
    /// a light object: the PDF will produce random vectors directed only
    /// towards the light, which reduces the noise coming from rays
    /// bouncing around the box and never finding it.
    class HittablePDF: public PDF
    {
        public:

            HittablePDF(std::shared_ptr<Hittable> obj, const Vec3& origin):
                obj(obj), origin(origin) {}

            virtual Vec3 random_vector() const override
            {
                // A random vector directed at a Hittable is a vector
                // directed at a random point on the surface of the
                // Hittable.
                return obj->random_point(origin);
            }

            virtual float val(const Vec3& dir) const override
            {
                // The probability that the ray {origin, dir} touches
                // the Hittable object.
                return obj->pdf_value(origin, dir);
            }

        public:

            std::shared_ptr<Hittable> obj;
            Vec3 origin;
    };

    /// Utility class for mixing different PDFs in order to control the
    /// exact kind of importance sampling we want for the scene.
    class MixturePDF: public PDF
    {
        public:

            MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1):
                    p0(p0), p1(p1) {}

            virtual Vec3 random_vector() const override
            {
                if(random_float() < 0.5f)
                    return p0->random_vector();
                else
                    return p1->random_vector();
            }

            virtual float val(const Vec3& dir) const override
            {
                return 0.5f*(p0->val(dir) + p1->val(dir));
            }

        public:

            std::shared_ptr<PDF> p0, p1;
    };
}