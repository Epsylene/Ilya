
#include "Material.hpp"
#include "Hittable.hpp"
#include "Utils/Random.hpp"

namespace Ilya
{

    Vec3 reflect(const Vec3& v, const Vec3& n)
    {
        return v - 2*dot(v, n)*n;
    }

    Vec3 refract(const Vec3& v, const Vec3& n, float ratio)
    {
        auto c = std::min(dot(-v, n), 1.f);
        auto t_normal = ratio * (v + c*n);
        auto t_tangent = -std::sqrt(std::abs(1.f - square(t_normal))) * n;

        return t_normal + t_tangent;
    }

    bool Lambertian::scatter(const Ray& in, Ray& out, Color& albedo,
                             const HitRecord& rec, float& pdf) const
    {
        // Lambertian diffusion: contrary to specular reflection, where the
        // ray reflects off the surface at a precise angle, diffuse
        // reflection has rays scatter at many angles. Lambertian diffusion
        // is the ideal case of diffuse reflection, where the ray scatter
        // following a cosinus law. The scattering will then be done with
        // a random cosine law vector, as seen in the local basis of the
        // surface:
        ONB uvw {rec.normal};
        auto dir = uvw.local(Random::cosine_dir());

        out = {rec.p, unit(dir), in.cast_time};
        albedo = this->albedo->val(rec.u, rec.v, rec.p);
        // Lambert's cosine law can be rendered as the dot product of the
        // material's surface normal and the scattering ray direction,
        // because N·L = |N||L|cos(theta) = cos(theta), N and L being
        // unitary. Here the normal N is given by the w vector of the
        // surface's local basis, and the direction vector is 'out.dir'.
        // Normalizing by pi then gives the spatial PDF of the scattering
        // rays:
        pdf = dot(uvw.w, out.dir) / pi;

        return true;
    }

    float Lambertian::scattering_pdf(const Ray& in, const Ray& out,
                                     const HitRecord& rec) const
    {
        auto cos = dot(rec.normal, unit(out.dir));

        return cos < 0.f ? 0.f : cos/pi;
    }

    bool Metal::scatter(const Ray& in, Ray& out, Color& albedo,
                        const HitRecord& rec, float & pdf) const
    {
        // Specular reflection is simple: the ray scatters off the
        // surface at the same angle, with opposite direction.
        // The fuziness parameter allows to add a diffusive component to
        // our metallic material: the more fuziness, the more rays there is
        // that do not scatter exactly like expected, and the more the
        // object has a matte look to it.
        auto reflected = reflect(unit(in.dir), rec.normal);
        out = {rec.p, reflected + fuziness * Random::in_unit_sphere(), in.cast_time};
        albedo = this->albedo;

        // We only want scattering to happen if rays are indeed reflected
        // off the surface.
        return (dot(out.dir, rec.normal) > 0);
    }

    bool Dielectric::scatter(const Ray& in, Ray& out, Color& albedo,
                             const HitRecord& rec, float & pdf) const
    {
        albedo = {1.f, 1.f, 1.f};
        float ratio = rec.frontFace ? (1.f/refraction) : 1.f;

        auto udir = unit(in.dir);
        float c = std::min(dot(-udir, rec.normal), 1.f);
        float s = std::sqrt(1.f - c*c);

        Vec3 dir {};
        // On one hand, because of Snell's law, there can be no refraction
        // if (n/n')*sin i > 1. On the other hand, when considering rays
        // arriving at the dielectric, they will tipically separate in a
        // reflected and a refracted parts. Rather than create two new rays,
        // any incoming ray either reflects or refracts (under Snell's law
        // conditions) on a random basis that depends on the reflectance of
        // the material (which depends on the refractive indices and the
        // incoming angle of the ray): if the reflectance (which ranges from
        // 0 to 1) is bigger than a random number between 0 and 1, then the
        // ray is reflected off the surface.
        if(ratio * s > 1.f || reflectance(c, ratio) > random_float())
            dir = reflect(udir, rec.normal);
        else
            dir = refract(udir, rec.normal, ratio);

        out = {rec.p, dir, in.cast_time};

        return true;
    }

    bool Isotropic::scatter(const Ray& in, Ray& out, Color& albedo,
                            const HitRecord& rec, float & pdf) const
    {
        // In an isotropic material, rays are scattered off uniformly
        // in all directions: thus the direction of the scattered ray
        // is simply a point in the unit sphere.
        out = {rec.p, Random::in_unit_sphere(), in.cast_time};
        albedo = this->albedo->val(rec.u, rec.v, rec.p);

        return true;
    }
}