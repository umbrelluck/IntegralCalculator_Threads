cmake_minimum_required(VERSION 3.0.0)
project(intCalculator VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 14)

add_executable(intCalculator program.cpp)

find_package(Threads REQUIRED)
target_link_libraries(intCalculator Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
