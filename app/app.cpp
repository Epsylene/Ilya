
#include "Core.hpp"

#include "Utils/Color.hpp"
#include "Objects/Instances.hpp"
#include "Objects/Camera.hpp"
#include "Core/Renderer.hpp"

using namespace Ilya;

int main()
{
    // Camera
    Camera cam {{278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 0.f, 10.f, 40.f, 1.f};
    const uint32_t width = 600;
    const uint32_t height = static_cast<int>(width / cam.aspect);
    const int samples_per_pixel = 200;
    const int depth = 25;

    HittableList world {};

    // Materials
    auto white = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.73f}));
    auto green = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.12f, 0.45f, 0.15f}));
    auto red = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.65f, 0.05f, 0.05f}));
    auto light_mat = std::make_shared<DiffuseLight>(15.f);
    auto metal = std::make_shared<Metal>(Color{0.8f, 0.85f, 0.88f}, 0.f);
    auto glass = std::make_shared<Dielectric>(1.5f);

    // Create and place the walls and light
    using Axis::X, Axis::Y, Axis::Z;
    world.add(std::make_shared<Rectangle<Y, Z>>(0, 0, 555, 555, 555, green));
    world.add(std::make_shared<Rectangle<Y, Z>>(0, 0, 555, 555, 0, red));
    world.add(std::make_shared<Rectangle<X, Z>>(0, 0, 555, 555, 0, white));
    world.add(std::make_shared<Rectangle<X, Z>>(0, 0, 555, 555, 555, white));
    world.add(std::make_shared<Rectangle<X, Y>>(0, 0, 555, 555, 555, white));

    auto light = std::make_shared<Rectangle<X, Z>>(213, 227, 343, 332, 554, light_mat);
    auto sphere = std::make_shared<Sphere>(Vec3{190, 90, 190}, 90, std::make_shared<Dielectric>(2.f));

    world.add(flip(light));
    world.add(sphere);

    auto lights = std::make_shared<HittableList>();
    lights->add(light);
    lights->add(sphere);

    // Create and place the boxes
    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Vec3{0, 0, 0}, Vec3{165, 330, 165}, metal);
    box1 = rotate<Y>(box1, 15);
    box1 = translate(box1, {265, 0, 295});

//    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Vec3{0,0,0}, Vec3{165,165,165}, white);
//    box2 = rotate<Y>(box2, -18);
//    box2 = translate(box2, {130, 0, 65});

    world.add(box1);
//    world.add(box2);

    world = HittableList{std::make_shared<BVHnode>(world)};

    // Render the image
    Renderer r {Image{width, height}, world, samples_per_pixel, depth};
    r.render(cam, lights);

    return 0;
}
