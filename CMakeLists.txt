cmake_minimum_required(VERSION 3.5)

project(3d-raytracer)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

set(lib_include_dirs ${SDL2_INCLUDE_DIR})
set(lib_libraries ${SDL2_LIBRARIES})

include_directories(${lib_include_dirs})

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

file(GLOB proj_source_files "${source_dir}/*.cpp" "${source_dir}/*/*.cpp")
file(GLOB proj_include_files "${source_dir}/*.hpp" "${source_dir}/*/*.hpp")

add_executable(3d-raytracer ${proj_source_files} ${proj_include_files})

target_link_libraries(3d-raytracer ${lib_libraries})
