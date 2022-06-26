cmake_minimum_required(VERSION 3.16)
project(NoisyBeauty)

find_package(OpenCV 4.5 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


set(CMAKE_CXX_STANDARD 14)

add_executable(NoisyBeauty main.cpp NoiseTransformer/NoiseTransformer.hpp CPP_UTILS/Matrix.h)
target_link_libraries(NoisyBeauty ${OpenCV_LIBS})
