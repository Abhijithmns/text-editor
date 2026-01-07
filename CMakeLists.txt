cmake_minimum_required(VERSION 3.23)

project(editor C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# executable
add_executable(editor
    gap_buffer.c
)

