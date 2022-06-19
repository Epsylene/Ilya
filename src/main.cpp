
#include "Ilya.hpp"

#include "Utils/Color.hpp"
#include "Objects/Ray.hpp"
#include "Objects/Hittable.hpp"
#include "Objects/Camera.hpp"

using namespace Ilya;

void write_Color(std::ofstream& out, const Color& color, int samples_per_pixel)
{
    // Colors are scaled inversely proportional to the samples per
    // pixel: antialiasing works by taking a pixel and sampling a great
    // number of neighboring pixels into it; this gives a blurry effect
    // at the edges, where colours change rapidly from one pixel to
    // another. However, because of the sampling, we have to tone down
    // each pixel's Color, or else the resulting antialised pixel will
    // be way brighter than it actually is.
    // However, our image is darker than it should be, for another
    // reason. Human eyes perceive light in a non-linear fashion,
    // following an approximate power function. To account for this,
    // software perform what is known as gamma correction, elevating
    // the input at a power that lies tipically in the range 1.8 to
    // 2.2. Because image viewers expect images to have been
    // gamma-corrected, we need first to inverse gamma-correct them;
    // supposing that gamma = 2, we need to elevate to the power 1/2.
    auto scale = 1.f/samples_per_pixel;
    Color sc_Color = sqrt(color * scale);

    out << static_cast<int>(256 * std::clamp(sc_Color.r, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * std::clamp(sc_Color.g, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * std::clamp(sc_Color.b, 0.f, 0.999f)) << '\n';
}

Color ray_color(const Ray& r, const Color& background, const Hittable& world, int depth)
{
    HitRecord rec {};

    if(depth <= 0)
        return {};

    // Check if the ray hits the 'world' hittable, and bounce off
    // the surface with some Color attenuation (to approximate the
    // fact that part of the rays are being absorbed by the material).
    // On the other hand, the cast_time at which rays are casted is not
    // set exactly at 0: if we did so, a bunch of the rays that were
    // supposed to scatter off the surface actually would because of
    // floating point rounding errors start a little under the surface,
    // intersect with it and never get out. This would produce an image
    // riddled with random black pixels, an outcome known as "shadow
    // acne"; setting the cast_time at which the ray detection starts just
    // a bit after 0 gets rid of most of it spectacularly well.
    if(!world.hit(r, 0.001f, infinity, rec))
        return background;

    Ray scattered {};
    Color attenuation {};
    Color emitted = rec.material->emitted(rec.u, rec.v, rec.p);

    // If the ray doesn't scatter from the material, it means that
    // it is a emissive one (it produces light); then the color we
    // want to output is the color of the emitted light.
    if(!rec.material->scatter(r, scattered, attenuation, rec))
        return emitted;

    // In the other cases, we want to sum the ray color to the
    // emitted light's color.
    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);

    // We get the unit vector from the ray direction.
    // Its Y component ranges from -1 to 1, so t ranges
    // from 0 to 1, which we use as the parameter for
    // the colour gradient; the result is that the background
    // colour goes vertically from white to blue.
    Vec3 unit_dir = unit(r.dir);
    auto m = 0.5f*(unit_dir.y + 1.f);

    return (1-m)*Color(1.f) + m*Color(0.5f, 0.7f, 1.f);
}

int main()
{
    std::ofstream f {"../image.ppm"};

    Camera cam {{0, 0, 16}, {0, 0, 0}, {0, 1, 0}, 0.f, 10.f, 40.f, 1.f};
    const int width = 200;
    const int height = static_cast<int>(width / cam.aspect);
    const int samples_per_pixel = 100;
    const int depth = 50;

    HittableList world {};

    auto white = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color::White));
    auto green = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color::Green));
    auto red = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color::Red));
    auto light = std::make_shared<DiffuseLight>(std::make_shared<SolidColor>(Color{4.f}));

    using Axis::X, Axis::Y, Axis::Z;
    world.add(std::make_shared<Rectangle<X, Z>>(-4, -4, 4, 4, -4, white));
    world.add(std::make_shared<Rectangle<X, Z>>(-4, -4, 4, 4, 4, white));
    world.add(std::make_shared<Rectangle<X, Y>>(-4, -4, 4, 4, -4, white));
    world.add(std::make_shared<Rectangle<Y, Z>>(-4, -4, 4, 4, -4, green));
    world.add(std::make_shared<Rectangle<Y, Z>>(-4, -4, 4, 4, 4, red));
    world.add(std::make_shared<Rectangle<X, Z>>(-1, -1, 1, 1, 3.9, light));

    world = HittableList{std::make_shared<BVHnode>(world)};

    // Outputting an image file: we use a PPM file, which is a
    // simple format looking like
    //
    //      P3
    //      [columns] [rows]
    //      [max_Color_value]
    //      [pixel00] [pixel01] ... [pixel0n]
    //      [pixel10] ...
    //      ...
    //
    f << "P3\n" << width << ' ' << height
        << "\n255\n";

    for (int j = height - 1; j >= 0; --j)
    {
        std::cout << "Scanlines remaining: " << j << "\n";

        for (int i = 0; i < width; ++i)
        {
            Color pixel_Color {};

            for (int s = 0; s < samples_per_pixel; ++s)
            {
                // Colors are normalized to the range [0,1] by convention
                auto u = (i + random_float()) / (width - 1);
                auto v = (j + random_float()) / (height - 1);

                pixel_Color += ray_color(cam.ray(u, v), {}, world, depth);
            }

            write_Color(f, pixel_Color, samples_per_pixel);
        }
    }

    return 0;
}
