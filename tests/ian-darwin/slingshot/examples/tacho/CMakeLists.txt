# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_tacho
    EXECUTABLE
    SOURCES
        "tacho.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
