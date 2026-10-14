# CMake replacement for the archived make/imake build.
ian_program(ian_xpm_image_demo
    EXECUTABLE
    SOURCES
        "pixmap.c"
    LIBRARIES Xpm
)
