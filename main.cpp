
#include <fstream>
#include <iostream>

#include "Utils/Color.hpp"
#include "Utils/Vector3.hpp"
#include "Objects/Ray.hpp"

void write_color(std::ofstream& out, const Color& color)
{
    int ir = static_cast<int>(255.999 * color.r);
    int ig = static_cast<int>(255.999 * color.g);
    int ib = static_cast<int>(255.999 * color.b);

    out << ir << ' ' << ig << ' ' << ib << '\n';
}

Color ray_color(const Ray& r)
{
    // We get the unit vector from the ray direction.
    // Its Y component ranges from -1 to 1, so t ranges
    // from 0 to 1, which we use as the parameter for
    // the colour gradient; the result is that the background
    // colour goes vertically from white to blue.
    Vec3 unit_dir = unit(r.dir);
    auto t = 0.5f*(unit_dir.y + 1.f);

    return (1-t)*Color(1.f) + t*Color(0.5f, 0.7f, 1.f);
}

int main()
{
    std::ofstream f {"../image.ppm"};

    const float aspect = 16.f/9.f;
    const int width = 256;
    const int height = static_cast<int>(width / aspect);

    // Our viewport is a plane that ranges [-1,1] in height and
    // [-aspect, aspect] in width. The rays all come from the origin
    // and go towards this plane; the distance between the origin and
    // the plane is called the "focal length" (here set at 1).
    float viewport_height = 2.f;
    float viewport_width = aspect * viewport_height;
    float focal = 1.f;

    Vec3 orig {}; // the origin point
    Vec3 horizontal {viewport_width, 0.f, 0.f};
    Vec3 vertical {0.f, viewport_height, 0.f};
    Vec3 llc = orig - horizontal/2.f - vertical/2.f - Vec3(0.f, 0.f, focal);
        // the vector going from the origin to the lower left corner
        // of the viewport (note that it is -Vec3(0.f, 0.f, focal) because
        // the Z axis points outwards the viewport plane)

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
            // Colors are normalized to the range [0,1] by convention
            auto u = (float)i/(width - 1);
            auto v = (float)j/(height - 1);

            // The rays scan the entire viewport, starting from the
            // lower left corner horizontally and then vertically
            Ray r {orig, llc + u*horizontal + v*vertical - orig};
            write_color(f, ray_color(r));
        }
    }

    return 0;
}
