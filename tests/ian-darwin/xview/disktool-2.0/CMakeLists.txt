# CMake replacement for the archived make/imake build.
ian_program(ian_disktool_2_0
    EXECUTABLE
    SOURCES
        "disktool.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
        "Linux"
)
