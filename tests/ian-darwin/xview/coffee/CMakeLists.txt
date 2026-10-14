# CMake replacement for the archived make/imake build.
ian_program(ian_coffee
    EXECUTABLE
    SOURCES
        "coffee.c"
        "xview.c"
        "util.c"
    DEFINITIONS
        "STANDALONE"
)
