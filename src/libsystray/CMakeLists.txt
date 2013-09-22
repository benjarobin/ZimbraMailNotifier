# Set the name and the supported language of the project
project(zimbrasystray C)
# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.8)
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK2_CFLAGS_OTHER})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wunused -Wshadow -Wcast-qual -fvisibility=hidden")
# Add sources files
aux_source_directory(. SRC_LIST)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
# Link the target to the GTK+ libraries
target_link_libraries(${PROJECT_NAME} ${GTK2_LIBRARIES})
