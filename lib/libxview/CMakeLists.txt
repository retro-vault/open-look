# lib/libxview/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# Canonical per-module source lists from Imake SRCS declarations.
include(${CMAKE_CURRENT_SOURCE_DIR}/sources.cmake)

add_library(xview SHARED ${XVIEW_SOURCES})
add_library(xview_static STATIC ${XVIEW_SOURCES})

# Linux-oriented compile definitions used by the historical build.
# xview relies on pixrect/olgx and X11 APIs.
find_package(X11 REQUIRED)
find_library(TIRPC_LIBRARY NAMES tirpc)

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
    target_include_directories(${target}
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
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
