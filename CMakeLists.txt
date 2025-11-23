cmake_minimum_required(VERSION 3.14)

project(stslog LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(STSLOG_BUILD_EXAMPLES OFF)
set(STSLOG_BUILD_TESTS OFF)
add_subdirectory(external/stslog)

add_executable(main ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main stslog::stslog)
target_include_directories(main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
