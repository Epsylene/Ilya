
#include "Material.hpp"
#include "Hittable.hpp"

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

bool Lambertian::scatter(const Ray& in, Ray& out, Color& attenuation,
                         const HitRecord& rec) const
{
    // Lambertian diffusion : contrary to specular reflection,
    // where the ray reflects off the surface at a precise angle,
    // diffuse reflection has rays scatter at many different
    // angles. Lambertian diffusion is the ideal case of diffuse
    // reflection, where the ray scatter following a cosinus law.
    // Geometrically, this means that the ray flux has the shape
    // of a unit circle tangent to the scattering point. The
    // relation with the cosinus law can then be understood
    // when one considers the angle between the center of the
    // circle, the scattering point and a random point on the
    // surface of the sphere: when this angle is 0, the distance
    // between the scattering point and the point on the surface
    // is maximal, and when it is pi/2, it is minimal, just like
    // with the cosinus function. We thus need simply for our
    // new ray direction to place a random point on the surface
    // of this unit sphere, by offseting by the normal (which
    // is unit-length) and then adding a random unit vector.
    auto scattered = rec.normal + random_unit_vector();
    if(near_zero(scattered))
        scattered = rec.normal;

    out = {rec.p, scattered, in.cast_time};
    attenuation = albedo;

    return true;
}

bool Metal::scatter(const Ray& in, Ray& out, Color& attenuation,
                    const HitRecord& rec) const
{
    // Specular reflection is simple: the ray scatters off the
    // surface at the same angle, with opposite direction.
    // The fuziness parameter allows to add a diffusive component to
    // our metallic material: the more fuziness, the more rays there is
    // that do not scatter exactly like expected, and the more the
    // object has a matte look to it.
    auto reflected = reflect(unit(in.dir), rec.normal);
    out = {rec.p, reflected + fuziness * rand_in_unit_sphere(), in.cast_time};
    attenuation = albedo;

    // We only want scattering to happen if rays are indeed reflected
    // off the surface.
    return (dot(out.dir, rec.normal) > 0);
}

bool Dielectric::scatter(const Ray& in, Ray& out, Color& attenuation,
                         const HitRecord& rec) const
{
    attenuation = {1.f, 1.f, 1.f};
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
