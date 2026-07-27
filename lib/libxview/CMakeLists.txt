# lib/libxview/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# Canonical per-module source lists from Imake SRCS declarations.
include(${CMAKE_CURRENT_SOURCE_DIR}/sources.cmake)

add_library(xview SHARED ${XVIEW_SOURCES})
add_library(xview_static STATIC ${XVIEW_SOURCES})

# Linux-oriented compile definitions used by the historical build.
# xview relies on pixrect/olgx and X11 APIs.
find_package(X11 REQUIRED)

# libtirpc provides <rpc/rpc.h>. On modern glibc (Debian 13+/many others)
# the headers live under /usr/include/tirpc, not /usr/include/rpc.
# PATH_SUFFIXES tirpc finds either layout.
find_path(TIRPC_INCLUDE_DIR
    NAMES rpc/rpc.h
    PATH_SUFFIXES tirpc
)
find_library(TIRPC_LIBRARY NAMES tirpc)
if(NOT TIRPC_INCLUDE_DIR OR NOT TIRPC_LIBRARY)
    message(FATAL_ERROR
        "libtirpc not found (need rpc/rpc.h and -ltirpc).\n"
        "  Debian/Ubuntu: sudo apt install libtirpc-dev\n"
        "  Fedora/RHEL:   sudo dnf install libtirpc-devel\n"
        "  Arch:          sudo pacman -S libtirpc\n"
        "Headers are usually at /usr/include/tirpc/rpc/rpc.h "
        "(requires -I/usr/include/tirpc).")
endif()
message(STATUS "libtirpc include: ${TIRPC_INCLUDE_DIR}")
message(STATUS "libtirpc library: ${TIRPC_LIBRARY}")

foreach(target xview xview_static)
    # Linux-oriented compile definitions used by the historical build.
    target_compile_definitions(${target}
        PRIVATE
            linux
            __amd64__
            NO_CAST_VATOAV
            _XV_API_BROKEN_64BIT
            _POSIX_C_SOURCE=199309L
            _POSIX_SOURCE
            _XOPEN_SOURCE
            _DEFAULT_SOURCE
            _LARGEFILE_SOURCE
            _FILE_OFFSET_BITS=64
    )

    # Public includes are exposed through include/xview and include/xview_private.
    # TIRPC is PUBLIC because xview headers (e.g. sel_svc.h) include <rpc/rpc.h>.
    target_include_directories(${target}
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
            ${TIRPC_INCLUDE_DIR}
    )
endforeach()

target_link_libraries(xview
    PUBLIC
        olgx
        ${TIRPC_LIBRARY}
        ${X11_LIBRARIES}
)

target_link_libraries(xview_static
    PUBLIC
        olgx_static
        ${TIRPC_LIBRARY}
        ${X11_LIBRARIES}
)

set_target_properties(xview PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    OUTPUT_NAME xview
)

set_target_properties(xview_static PROPERTIES
    OUTPUT_NAME xview
)

install(TARGETS xview xview_static
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/xview/
    DESTINATION include/xview
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/xview_private/
    DESTINATION include/xview_private
)
