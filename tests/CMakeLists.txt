cmake_minimum_required(VERSION 3.22)

project(tests VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -coverage")

set(libcornerinclude "${PROJECT_SOURCE_DIR}/../libcorner/include")
set(libcornersource "${PROJECT_SOURCE_DIR}/../libcorner/src")

find_package(OpenCV REQUIRED)

add_library(libcorner "${libcornerinclude}/libcorner.hpp" "${libcornersource}/libcorner.cpp")
target_include_directories(libcorner PRIVATE ${libcornerinclude})
target_link_libraries(libcorner PRIVATE ${OpenCV_LIBS})

find_package(Catch2 REQUIRED)
add_executable(tests tests.cpp)
target_include_directories(tests PRIVATE ${libcornerinclude})
target_link_libraries(tests libcorner ${OpenCV_LIBS} Catch2::Catch2WithMain)
