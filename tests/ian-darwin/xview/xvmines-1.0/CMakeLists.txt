# CMake replacement for the archived make/imake build.
ian_program(ian_xvmines_1_0
    EXECUTABLE
    SOURCES
        "xvmines.c"
        "play.c"
    DEFINITIONS
        "STANDALONE"
)
