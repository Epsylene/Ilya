
#pragma once

#include "Core.hpp"
#include "Random.hpp"
#include "Objects/Hittable.hpp"

namespace Ilya
{
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

            /// Gives the value of the PDF for the vector `dir`.
            virtual float val(const Vec3& dir) const = 0;
    };

    /// Cosine distribution PDF, used for example in Lambertian
    /// materials.
    class CosinePDF: public PDF
    {
        public:

            explicit CosinePDF(const Vec3& w): uvw(w) {}

            Vec3 random_vector() const override
            {
                // Generate a random vector following a cosine
                // distribution in the local basis:
                return uvw.local(Random::cosine_dir());
            }

            float val(const Vec3& dir) const override
            {
                // If the cosine is nonpositive, that is, if the angle
                // between the random vector and the surface normal is
                // greater than pi/2 (which means that the random vector
                // is directed towards the surface), return 0; in the
                // other case, return the cosine (given by Lambert's
                // cosine law) divided by pi, which is the value of the
                // probability for the cosine distribution over [0, pi].
                auto cos = dot(normalize(dir), uvw.w);
                return (cos <= 0) ? 0 : cos/pi;
            }

        private:

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

            HittablePDF(const Ref<Hittable>& obj, const Vec3& origin):
                    obj(obj), origin(origin) {}

            Vec3 random_vector() const override
            {
                // A random vector directed at a Hittable is a vector
                // directed at a random point on the surface of the
                // Hittable.
                return obj->random_point(origin);
            }

            float val(const Vec3& dir) const override
            {
                // The probability that the ray {origin, dir} touches
                // the Hittable object.
                return obj->pdf_value({origin, dir});
            }

        private:

            Ref<Hittable> obj;
            Vec3 origin;
    };

    /// Utility class for mixing different PDFs in order to control the
    /// exact kind of importance sampling we want for the scene.
    class MixturePDF: public PDF
    {
        public:

            MixturePDF(const Ref<PDF>& p0, const Ref<PDF>& p1):
                    p0(p0), p1(p1) {}

            Vec3 random_vector() const override
            {
                if(Random::rfloat() < 0.5f)
                    return p0->random_vector();
                else
                    return p1->random_vector();
            }

            float val(const Vec3& dir) const override
            {
                return 0.5f*(p0->val(dir) + p1->val(dir));
            }

        public:

            Ref<PDF> p0, p1;
    };
}