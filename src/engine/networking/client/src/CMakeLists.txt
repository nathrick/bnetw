cmake_minimum_required(VERSION 3.9)

set(INCLUDE_DIRS_CLIENT ${PROJECT_SOURCE_DIR}/src/engine/networking ${PROJECT_SOURCE_DIR}/src/api)
set(USED_LIBS_CLIENT ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} -lboost_serialization -lboost_system -lpthread -lm)
set(SOURCE_FILES_CLIENT client.cpp)
set(LIB_NAME "bnetw_client")

include(GNUInstallDirs)
include_directories(${INCLUDE_DIRS_CLIENT})

add_library(${LIB_NAME} SHARED ${SOURCE_FILES_CLIENT})
set_target_properties(${LIB_NAME} PROPERTIES 
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/src/api/bnetw_api.hpp
)
target_include_directories(${LIB_NAME}  PRIVATE .)
target_link_libraries(${LIB_NAME}  ${USED_LIBS_CLIENT})

install(TARGETS ${LIB_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


