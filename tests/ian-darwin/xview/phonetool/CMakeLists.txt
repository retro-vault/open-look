# CMake replacement for the archived make/imake build.
ian_program(ian_phonetool
    EXECUTABLE
    SOURCES
        "phonetool_data.c"
        "phonetool_build.c"
        "phonetool.c"
    DEFINITIONS
        "STANDALONE"
)
