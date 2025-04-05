# lib/olgx/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

option(BUILD_SHARED_LIBS "Build olgx as a shared library" ON)

# Source files
set(SOURCES
    ol_button.c
    ol_color.c
    ol_draw.c
    ol_init.c
    ol_misc.c
    ol_sb.c
    ol_slider.c
)

# Private headers (not installed)
set(HEADERS_PRIVATE
    busy.h
    grey.h
    olgx_impl.h
    ol_button_.h
    ol_color_.h
    ol_draw_.h
    ol_init_.h
    ol_misc_.h
    ol_sb_.h
    ol_slider_.h
)

# Define the library
add_library(olgx ${SOURCES})

# Link against X11
find_package(X11 REQUIRED)
target_link_libraries(olgx PRIVATE ${X11_LIBRARIES})

# Set version and output name
set_target_properties(olgx PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    OUTPUT_NAME olgx
)

# Public header is now in <root>/include/olgx/
set(HEADERS_PUBLIC
    ${PROJECT_SOURCE_DIR}/include/olgx/olgx.h
)

# Include directories
target_include_directories(olgx
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the library
install(TARGETS olgx
    EXPORT olgxTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install public headers
install(FILES ${HEADERS_PUBLIC}
    DESTINATION include/olgx
)
