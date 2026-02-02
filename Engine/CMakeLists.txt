cmake_minimum_required(VERSION 3.22)
project(TitanEngine)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include folder for headers
include_directories(include)

# All source files
set(SOURCES
    main.cpp
    Entity.cpp
    Input.cpp
    Physics.cpp
)

# Create executable
add_executable(TitanEngine ${SOURCES})

# Link Raylib (adjust path if your lib folder is custom)
target_link_libraries(TitanEngine PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lib/raylib.lib
    user32
    gdi32
    winmm
)
