cmake_minimum_required(VERSION 3.1)
project(fmesh)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigureAllModules) 

if (WIN32)
else()
    set(CMAKE_CXX_FLAGS "-fPIC")
    set(CMAKE_C_FLAGS "-fPIC")
endif()

__find_trimesh2()
__find_clipper()

add_subdirectory(mmesh/external mmesh)
add_subdirectory(src)