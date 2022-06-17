cmake_minimum_required(VERSION 3.22)
project(Ilya)

set(CMAKE_CXX_STANDARD 20)

add_executable(Ilya main.cpp Utils/Vector4.hpp Utils/Color.cpp Objects/Ray.hpp Utils/Vector3.cpp Objects/Hittable.cpp Objects/Hittable.hpp Ilya.hpp Objects/Camera.cpp Objects/Camera.hpp Objects/Material.cpp Objects/Material.hpp Objects/BoundingBox.hpp Objects/BoundingBox.cpp Objects/Texture.hpp Utils/Perlin.hpp)

target_include_directories(Ilya PUBLIC ${CMAKE_SOURCE_DIR})
target_precompile_headers(Ilya PUBLIC Ilya.hpp)
target_compile_options(Ilya PUBLIC -O3 -fno-math-errno)