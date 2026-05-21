# set(ENV{LD_LIBRARY_PATH} "LD_LIBRARY_PATH=/home/titan_sw/GCC4.7.2/LINUX_GCC/lib64")
# set(CMAKE_CXX_COMPILER "/home/titan_sw/GCC4.7.2/LINUX_GCC/bin/g++")
cmake_minimum_required(VERSION 2.8)
project("test pwl")

set(testoption "testoption" CACHE STRING "default_value")


add_executable(test_pwl test_pwl_main.cpp ../pwl_line.cpp)
add_executable(test test.cpp ../pwl_line.cpp first_test.cpp)


include_directories(../../)


