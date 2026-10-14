# CMake replacement for the archived make/imake build.
ian_program(ian_psdraw
    EXECUTABLE
    SOURCES
        "psdraw.c"
        "psdraw_ui.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
