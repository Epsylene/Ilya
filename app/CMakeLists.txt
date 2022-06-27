cmake_minimum_required(VERSION 3.22)
project(App)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

add_executable(Ilya_app app.cpp)

target_link_libraries(Ilya_app Ilya)
target_include_directories(Ilya_app PUBLIC ${ILYA_INCLUDE_DIRS})
target_compile_options(Ilya_app PUBLIC -O3 -fno-math-errno)

set(ILYA_BIN_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

add_custom_command(
        TARGET Ilya_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${ILYA_BIN_DIR}/libIlya.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libIlya.dll
)