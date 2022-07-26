
#include "Core.hpp"

#include "Utils/Color.hpp"
#include "Objects/Instances.hpp"
#include "Objects/Camera.hpp"
#include "Core/Renderer.hpp"

using namespace Ilya;

int main()
{
    Camera cam {{278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 0.f, 10.f, 40.f, 1.f};
    const uint32_t width = 200;
    const uint32_t height = static_cast<int>(width / cam.aspect);
    const int samples_per_pixel = 400;
    const int depth = 25;

    HittableList world {};

    auto white = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.73f}));
    auto green = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.12f, 0.45f, 0.15f}));
    auto red = std::make_shared<Lambertian>(std::make_shared<SolidColor>(Color{0.65f, 0.05f, 0.05f}));
    auto light = std::make_shared<DiffuseLight>(std::make_shared<SolidColor>(Color{15.f}));

    using Axis::X, Axis::Y, Axis::Z;
    world.add(std::make_shared<Rectangle<Y, Z>>(0, 0, 555, 555, 555, green));
    world.add(std::make_shared<Rectangle<Y, Z>>(0, 0, 555, 555, 0, red));
    world.add(std::make_shared<Rectangle<X, Z>>(213, 227, 343, 332, 554, light));
    world.add(std::make_shared<Rectangle<X, Z>>(0, 0, 555, 555, 0, white));
    world.add(std::make_shared<Rectangle<X, Z>>(0, 0, 555, 555, 555, white));
    world.add(std::make_shared<Rectangle<X, Y>>(0, 0, 555, 555, 555, white));

    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Vec3{0, 0, 0}, Vec3{165, 330, 165}, white);
    box1 = std::make_shared<Rotate<Y>>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3{265,0,295});

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Vec3{0,0,0}, Vec3{165,165,165}, white);
    box2 = std::make_shared<Rotate<Y>>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3{130,0,65});

    world.add(std::make_shared<ConstantMedium>(box1, Color{0,0,0}, 0.01));
    world.add(std::make_shared<ConstantMedium>(box2, Color{1,1,1}, 0.01));

    world = HittableList{std::make_shared<BVHnode>(world)};

    Renderer r {Image{width, height}, world, samples_per_pixel, depth};
    r.render(cam);

    return 0;
}
