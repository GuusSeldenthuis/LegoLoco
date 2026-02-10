cmake_minimum_required(VERSION 3.15)
project(lego_loco VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
add_subdirectory(raylib)

# Source files
file(GLOB SOURCES src/*.cpp)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread dl rt X11)
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()
