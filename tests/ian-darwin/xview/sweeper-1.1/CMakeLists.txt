# CMake replacement for the archived make/imake build.
ian_program(ian_sweeper_1_1
    EXECUTABLE
    SOURCES
        "sweep.c"
        "window.c"
    DEFINITIONS
        "STANDALONE"
)
