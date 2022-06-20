cmake_minimum_required(VERSION 3.22)
project(Ilya)

set(CMAKE_CXX_STANDARD 20)

add_library(stb_image STATIC lib/stb_image/stb_image.cpp lib/stb_image/stb_image.h)
target_include_directories(stb_image PUBLIC lib/stb_image)

add_executable(Ilya src/main.cpp src/Utils/Vector4.hpp src/Utils/Color.cpp src/Objects/Ray.hpp src/Utils/Vector3.cpp src/Objects/Hittable.cpp src/Objects/Hittable.hpp src/Ilya.hpp src/Objects/Camera.hpp src/Objects/Material.cpp src/Objects/Material.hpp src/Objects/BoundingBox.hpp src/Objects/BoundingBox.cpp src/Objects/Texture.hpp src/Utils/Perlin.hpp src/Objects/Instances.cpp src/Objects/Instances.hpp)

target_include_directories(Ilya PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Ilya PUBLIC stb_image)

target_precompile_headers(Ilya PUBLIC src/Ilya.hpp)
target_compile_options(Ilya PUBLIC -O3 -fno-math-errno)

target_compile_definitions(Ilya PUBLIC -DILYA_RES_DIR=${CMAKE_SOURCE_DIR}/res)