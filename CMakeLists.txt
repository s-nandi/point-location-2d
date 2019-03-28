cmake_minimum_required(VERSION 3.1)
project (Quadedge)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
set(SOURCE
        tester.cpp
        src/edge.cpp
        src/lawson_oriented_walk.cpp
        src/naive_quadtree.cpp
        src/parsing.cpp
        src/plane.cpp
        src/point2D.cpp
        src/quadedge.cpp
        src/quadtree.cpp
        src/slab_decomposition.cpp
        src/starting_edge_selector.cpp
        src/triangulation.cpp
        src/uniform_point_rng.cpp
        src/vertex.cpp
        src/walking_point_location.cpp)
add_executable(Quadedge ${SOURCE})

