# CMake replacement for the archived make/imake build.
ian_program(ian_painter
    EXECUTABLE
    SOURCES
        "painter.c"
    DEFINITIONS
        "STANDALONE"
)
