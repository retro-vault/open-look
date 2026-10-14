# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_drawtext
    EXECUTABLE
    SOURCES
        "drawtext.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
