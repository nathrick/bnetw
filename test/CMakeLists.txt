cmake_minimum_required(VERSION 3.9)

set(BINARY ${CMAKE_PROJECT_NAME}_test)
set(TEST_SOURCES main.cpp connection_test.cpp server_test.cpp ${PROJECT_SOURCE_DIR}/src/engine/networking/server/src/server.cpp)

set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/engine/networking ${PROJECT_SOURCE_DIR}/src/api)

include_directories(${INCLUDE_DIRS})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC -lgtest -lpthread -lboost_serialization -lboost_system -lboost_thread)