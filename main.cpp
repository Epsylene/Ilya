
#include <fstream>
#include <iostream>

#include "Utils/Color.hpp"

void write_color(std::ofstream& out, const Color& color)
{
    int ir = static_cast<int>(255.999 * color.r);
    int ig = static_cast<int>(255.999 * color.g);
    int ib = static_cast<int>(255.999 * color.b);

    out << ir << ' ' << ig << ' ' << ib << '\n';
}

int main()
{
    std::ofstream f {"../image.ppm"};

    const int width = 256;
    const int height = 256;

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
            // Colors are first normalized to the range [0,1]
            // by convention...
            Color c { (float)i/(width - 1),
                      (float)j/(height - 1),
                      0.25 };

            write_color(f, c);
        }
    }

    return 0;
}
