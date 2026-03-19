cmake_minimum_required(VERSION 3.10)
project(GameBoyCore)

set(PROJECT_SOURCE_FILES 
    src/emu.cpp
    src/cpu.cpp
    src/ppu.cpp
    src/cart.cpp
)

# Find SDL2 via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Include directories
include_directories(
    src
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Create library
add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
