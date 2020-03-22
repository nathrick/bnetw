cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(USED_LIBS_EXAMPLE "-lboost_system -lpthread -lm -lbnetw_client")
set(SOURCE_FILES_EXAMPLE main.cpp)
set(EXECUTABLE "bnetw_example")

link_directories(/usr/local/lib)

add_executable(${EXECUTABLE} ${SOURCE_FILES_EXAMPLE})

target_link_libraries(${EXECUTABLE} ${USED_LIBS_EXAMPLE})
