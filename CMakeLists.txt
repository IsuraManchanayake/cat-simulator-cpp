cmake_minimum_required(VERSION 3.20)
project(cat_simulator_cpp)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(lib/googletest)
#include_directories(lib/googletest/googletest/include)
#include_directories(lib/googletest/googlemock/include)
#
#add_executable(cat_simulator_cpp src/catsim/Cat.cpp src/catsim/Cat.h src/catsim/Vec2.cpp src/catsim/Vec2.h tests/vec2_test.cpp)
#target_link_libraries(cat_simulator_cpp gtest gtest_main)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(lib/googletest)