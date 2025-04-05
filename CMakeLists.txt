cmake_minimum_required(VERSION 3.16)
project(OpenLook C)

# Use Debug as default if none is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_C_STANDARD 99)

# Find required X11 libraries
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(src)
