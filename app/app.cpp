
#include "Core.hpp"

#include "Utils/Color.hpp"
#include "Objects/Instances.hpp"
#include "Objects/Camera.hpp"
#include "Core/Renderer.hpp"

using namespace Ilya;

int main()
{

    // Create the camera
    Camera cam {{278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 0.f, 10.f, 40.f, 1.f};
    const uint32_t width = 400;
    const uint32_t height = static_cast<int>(width / cam.aspect);
    const int samples_per_pixel = 100;
    const int depth = 25;

    HittableList world {};

    // Create the materials
    auto white = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.73f}));
    auto green = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.12f, 0.45f, 0.15f}));
    auto red = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.65f, 0.05f, 0.05f}));
    auto light = std::make_shared<DiffuseLight>(15.f);

    // Create and place the walls
    using Axis::X, Axis::Y, Axis::Z;
    world.add(std::make_shared<Rectangle<Y, Z>>(0, 0, 555, 555, 555, green));
    world.add(std::make_shared<Rectangle<Y, Z>>(0, 0, 555, 555, 0, red));
    world.add(std::make_shared<Rectangle<X, Z>>(213, 227, 343, 332, 554, light));
    world.add(std::make_shared<Rectangle<X, Z>>(0, 0, 555, 555, 0, white));
    world.add(std::make_shared<Rectangle<X, Z>>(0, 0, 555, 555, 555, white));
    world.add(std::make_shared<Rectangle<X, Y>>(0, 0, 555, 555, 555, white));

    // Create and place the boxes
    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Vec3{0, 0, 0}, Vec3{165, 330, 165}, white);
    box1 = std::make_shared<Rotate<Y>>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3{265,0,295});

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Vec3{0,0,0}, Vec3{165,165,165}, white);
    box2 = std::make_shared<Rotate<Y>>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3{130,0,65});

    world.add(box1);
    world.add(box2);

    world = HittableList{std::make_shared<BVHnode>(world)};

    Renderer r {Image{width, height}, world, samples_per_pixel, depth};
    r.render(cam);

//    // To estimate the value of Pi, one can use a Monte-Carlo method:
//    // we know the area of the circle is related to Pi by the formula
//    // A_c = pi*r^2, where r is the radius. Then, inscribing the circle
//    // inside a square, the ratio A_c/A_s = pi*r^2/(2r)^2 = pi/4, so
//    // pi = 4*A_c/A_s. But calculating an area is the same as
//    // calculating the number of points inside that area: then we
//    // simply need to (1) create random points inside a given square,
//    // (2) check if the points are inside the inscribed circle too,
//    // and (3) calculate the ratio between these two numbers.
//
//    int in_square = 0;
//    int in_circle = 0;
//
//    while(true)
//    {
//        in_square++;
//
//        // We get a random position (x, y) in the [-1, 1]x[-1,1]
//        // square...
//        auto x = Random::rfloat(-1, 1);
//        auto y = Random::rfloat(-1, 1);
//
//        // ...and check if it is inside the unit circle.
//        if(x*x + y*y < 1)
//            in_circle++;
//
//        std::cout << "pi = " << 4*(double)in_circle/in_square << "\n";
//    }

//    // Let's say that we want to integrate some function f. We could go
//    // analytically, using its integrand F, or numerically, with a sum over
//    // the areas of a number of small rectangles under the function; a third
//    // way would be using random numbers. The idea is to sum a number of
//    // times f(r), where r is a random number in the range of integration,
//    // weighted by a PDF (Probability Density Function), which tells how
//    // the random numbers should be distributed (or how much importance
//    // each region has, which is why it is also called "importance sampling")
//
//    auto pdf = [](double x){ return 0.5*x; };
//
//    auto N = 10000;
//    auto sum = 0.;
//    for (int i = 0; i < N; ++i)
//    {
//        auto x = std::sqrt(random_float(0, 4));
//        sum += x*x/pdf(x);
//    }
//
//    std::cout << "I = " << sum/N;

    return 0;
}
