# CMake replacement for the archived make/imake build.
ian_program(ian_scantool
    EXECUTABLE
    SOURCES
        "scantool.c"
        "graphics.c"
        "misc.c"
        "xview.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
)
