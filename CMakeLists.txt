cmake_minimum_required(VERSION 3.22)

project(corner VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(libcornerinclude "${PROJECT_SOURCE_DIR}/libcorner/include")
set(libcornersource "${PROJECT_SOURCE_DIR}/libcorner/src")

find_package(OpenCV 4.7.0 EXACT REQUIRED PATHS /home/policratus/.opencv-4.7.0)

add_library(libcorner STATIC "${libcornerinclude}/libcorner.hpp" "${libcornersource}/libcorner.cpp")
target_include_directories(libcorner PRIVATE ${libcornerinclude})
target_link_libraries(libcorner PRIVATE ${OpenCV_LIBS})

add_executable(corner corner.cpp)
target_include_directories(corner PRIVATE ${libcornerinclude})
target_link_libraries(corner PRIVATE ${OpenCV_LIBS} libcorner)
