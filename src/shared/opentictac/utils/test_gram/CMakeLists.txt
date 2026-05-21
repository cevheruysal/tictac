set(ENV{LD_LIBRARY_PATH} "LD_LIBRARY_PATH=/home/titan_sw/GCC4.7.2/LINUXAMD64/lib64")
# set(CMAKE_CXX_COMPILER "/home/titan_sw/GCC4.7.2/LINUX_GCC/bin/g++")
cmake_minimum_required(VERSION 3.1)
project("test gram")

include(CTest)

set(testoption "testoption" CACHE STRING "default_value" FORCE )

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)




# set(CMAKE_CXX_COMPILER "clang++" CACHE PATH "" FORCE)
# set(CMAKE_C_COMPILER "clang" CACHE PATH "" FORCE)

# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# add_executable(test_pwl test_pwl_main.cpp ../pwl_line.cpp)
# add_executable(test test.cpp ../pwl_line.cpp first_test.cpp)


add_library(pegtl INTERFACE)
target_include_directories(pegtl INTERFACE ../tao/)
target_compile_options(pegtl INTERFACE $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>)



add_executable(manual_test test_main.cpp )
target_link_libraries(manual_test pegtl)

add_executable(main_test main_test.cpp )
target_link_libraries(main_test pegtl)

add_test( NAME MainTest COMMAND main_test [devString])
add_test( NAME SecondTest COMMAND main_test [devString2])


include_directories(../../)
include_directories(../)


