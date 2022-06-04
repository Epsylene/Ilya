
#include <fstream>
#include <iostream>

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
            auto r = (double)i/(width - 1);
            auto g = (double)j/(height - 1);
            auto b = 0.25;

            // ...and then to [0,255] for the PPM file.
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            f << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    return 0;
}
