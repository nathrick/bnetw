cmake_minimum_required(VERSION 3.9)

set(INCLUDE_DIRS_CLIENT /usr/local/include)
set(USED_LIBS_CLIENT ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} -lboost_serialization -lboost_system -lpthread -lm -lgsdkclient)
set(SOURCE_FILES_CLIENT main.cpp)
set(EXECUTABLE "gsdk_example")

include_directories(${INCLUDE_DIRS_CLIENT})
link_directories(/usr/local/lib)
add_executable(${EXECUTABLE} ${SOURCE_FILES_CLIENT})
target_link_libraries(${EXECUTABLE} ${USED_LIBS_CLIENT})