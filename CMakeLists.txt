cmake_minimum_required(VERSION 3.16)
project(OpenLook C)

# Use Debug as default if none is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_C_STANDARD 99)

# Keep all final build artifacts in <repo>/bin while object files and
# intermediate state remain under the selected build tree (e.g. build/).
set(OPENLOOK_STAGE_DIR "${CMAKE_SOURCE_DIR}/bin")
set(OPENWINHOME_STAGE_DIR "${OPENLOOK_STAGE_DIR}")
set(OPENWINHOME_STAGE_BIN_DIR "${OPENWINHOME_STAGE_DIR}/bin")
set(OPENWINHOME_STAGE_LIB_DIR "${OPENWINHOME_STAGE_DIR}/lib")
set(OPENWINHOME_STAGE_INCLUDE_DIR "${OPENWINHOME_STAGE_DIR}/include")
set(OPENWINHOME_STAGE_SHARE_DIR "${OPENWINHOME_STAGE_DIR}/share")

file(MAKE_DIRECTORY "${OPENWINHOME_STAGE_BIN_DIR}")
file(MAKE_DIRECTORY "${OPENWINHOME_STAGE_LIB_DIR}")
file(MAKE_DIRECTORY "${OPENWINHOME_STAGE_INCLUDE_DIR}")
file(MAKE_DIRECTORY "${OPENWINHOME_STAGE_SHARE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OPENWINHOME_STAGE_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OPENWINHOME_STAGE_LIB_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OPENWINHOME_STAGE_LIB_DIR}")

foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    string(TOUPPER "${cfg}" cfg_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg_upper} "${OPENWINHOME_STAGE_BIN_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg_upper} "${OPENWINHOME_STAGE_LIB_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg_upper} "${OPENWINHOME_STAGE_LIB_DIR}")
endforeach()

# Find required X11 libraries
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/resources)

# Provide a deterministic fallback OPENWINHOME path in this staged tree.
add_compile_definitions(OPENWINHOME_DEFAULT="${OPENWINHOME_STAGE_DIR}")

# Convenience shell snippet for running from the staged OPENWINHOME tree.
set(OPENWINHOME_ENV_SCRIPT "${OPENLOOK_STAGE_DIR}/use-openwinhome.sh")
file(WRITE "${OPENWINHOME_ENV_SCRIPT}"
"#!/usr/bin/env bash\n"
"# Source this file: source ./bin/use-openwinhome.sh\n"
"_owh_script=\"$BASH_SOURCE\"\n"
"if [ -z \"$_owh_script\" ]; then _owh_script=\"$0\"; fi\n"
"_owh_dir=\"$(cd \"$(dirname \"$_owh_script\")\" && pwd)\"\n"
"export OPENWINHOME=\"$_owh_dir\"\n"
"export PATH=\"$OPENWINHOME/bin:$PATH\"\n"
"if [ -n \"$LD_LIBRARY_PATH\" ]; then\n"
"  export LD_LIBRARY_PATH=\"$OPENWINHOME/lib:$LD_LIBRARY_PATH\"\n"
"else\n"
"  export LD_LIBRARY_PATH=\"$OPENWINHOME/lib\"\n"
"fi\n"
"echo \"OPENWINHOME=$OPENWINHOME\"\n"
)
execute_process(COMMAND chmod +x "${OPENWINHOME_ENV_SCRIPT}")

# Stage public development headers into OPENWINHOME/include.
add_custom_target(openlook_headers ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENWINHOME_STAGE_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/include/olgx"
            "${OPENWINHOME_STAGE_INCLUDE_DIR}/olgx"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/include/pixrect"
            "${OPENWINHOME_STAGE_INCLUDE_DIR}/pixrect"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/include/xview"
            "${OPENWINHOME_STAGE_INCLUDE_DIR}/xview"
    COMMENT "Staging OpenLook public headers into ${OPENWINHOME_STAGE_INCLUDE_DIR}"
)

# Add subdirectories
add_subdirectory(resources)
add_subdirectory(lib)
add_subdirectory(src)
