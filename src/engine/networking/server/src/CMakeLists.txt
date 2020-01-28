cmake_minimum_required(VERSION 2.8)

set(INCLUDE_DIRS_SERVER ${PROJECT_SOURCE_DIR}/src/engine/networking ${PROJECT_SOURCE_DIR}/src/api)
set(USED_LIBS_SERVER ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} -lboost_serialization -lboost_system -lpthread -lm)
set(SOURCE_FILES_SERVER server.cpp main.cpp)
set(EXECUTABLE_SERVER "${PROJECT_NAME}_server")

include_directories(${INCLUDE_DIRS_SERVER})
add_executable(${EXECUTABLE_SERVER} ${SOURCE_FILES_SERVER})
target_link_libraries(${EXECUTABLE_SERVER} ${USED_LIBS_SERVER})

