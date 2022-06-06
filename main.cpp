
#include "Ilya.hpp"

#include "Utils/Color.hpp"
#include "Objects/Ray.hpp"
#include "Objects/Hittable.hpp"
#include "Objects/Camera.hpp"

void write_color(std::ofstream& out, const Color& color, int samples_per_pixel)
{
    // Colors are scaled inversely proportional to the samples per
    // pixel: antialiasing works by taking a pixel and sampling a great
    // number of neighboring pixels into it; this gives a blurry effect
    // at the edges, where colours change rapidly from one pixel to
    // another. However, because of the sampling, we have to tone down
    // each pixel's color, or else the resulting antialised pixel will
    // be way brighter than it actually is.
    auto scale = 1.f/samples_per_pixel;
    auto sc_color = color * scale;

    out << static_cast<int>(256 * std::clamp(sc_color.r, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * std::clamp(sc_color.g, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * std::clamp(sc_color.b, 0.f, 0.999f)) << '\n';
}

Color ray_color(const Ray& r, const HittableList& world)
{
    HitRecord rec {};
    // Check if the ray hits the 'world' hittable, and shade
    // according to the normal components
    if(world.hit(r, 0, infinity, rec))
        return 0.5f * (rec.normal + Vec3{1.f});

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

    Camera cam {};
    const int width = 512;
    const int height = static_cast<int>(width / cam.aspect);
    const int samples_per_pixel = 25;

    HittableList world {};
    world.add(std::make_shared<Sphere>(Vec3{0.f, 0.f, -1.f}, 0.5f));
    world.add(std::make_shared<Sphere>(Vec3{0.f, -100.5f, -1.f}, 100.f));

    // Outputting an image file: we use a PPM file, which is a
    // simple format looking like
    //
    //      P3
    //      [columns] [rows]
    //      [max_color_value]
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
            Color pixel_color {};

            for (int s = 0; s < samples_per_pixel; ++s)
            {
                // Colors are normalized to the range [0,1] by convention
                auto u = (i + random_float()) / (width - 1);
                auto v = (j + random_float()) / (height - 1);

                pixel_color += ray_color(cam.ray(u, v), world);
            }

            write_color(f, pixel_color, samples_per_pixel);
        }
    }

    return 0;
}
