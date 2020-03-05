cmake_minimum_required(VERSION 3.0.0)
project(intCalculator VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 14)

set(CONF_LIB_NAME config_lib)
set(INT_LIB_NAME int_lib)
set(EXEC_NAME intCalculator)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(Threads)

add_library(${CONF_LIB_NAME} SHARED config/config.h config/config.cpp)
add_library(${INT_LIB_NAME} SHARED integral/integral.h integral/integral.cpp)

add_executable(${EXEC_NAME} main.cpp)
target_link_libraries(${EXEC_NAME} ${CONF_LIB_NAME})
target_link_libraries(${EXEC_NAME} ${INT_LIB_NAME})

target_link_libraries(${EXEC_NAME} ${CMAKE_THREAD_LIBS_INIT})
