cmake_minimum_required(VERSION 3.9)

project(bnetw VERSION 0.0.1 DESCRIPTION "Network client-server library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost REQUIRED COMPONENTS system thread regex)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

if(BUILD_TEST)
add_subdirectory(test)
else()
add_subdirectory(src)
endif()

add_custom_target(example
    COMMAND echo [----] Building example app
    USES_TERMINAL
    COMMAND cd ${PROJECT_SOURCE_DIR}/example 
            && mkdir -p build 
            && cd build
            && cmake ..
            && make
    )





