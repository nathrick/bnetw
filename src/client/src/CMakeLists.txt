cmake_minimum_required(VERSION 2.8)

set(INCLUDE_DIRS_CLIENT ${PROJECT_SOURCE_DIR}/src)
set(USED_LIBS_CLIENT ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} -lboost_serialization -lboost_system -lpthread -lm)
set(SOURCE_FILES_CLIENT ${PROJECT_SOURCE_DIR}/src/message.cpp client.cpp main.cpp)
set(EXECUTABLE_CLIENT "${PROJECT_NAME}_client")

include_directories(${INCLUDE_DIRS_CLIENT})
add_executable(${EXECUTABLE_CLIENT} ${SOURCE_FILES_CLIENT})
target_link_libraries(${EXECUTABLE_CLIENT} ${USED_LIBS_CLIENT})

