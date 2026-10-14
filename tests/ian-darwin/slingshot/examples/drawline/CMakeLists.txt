# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_drawline
    EXECUTABLE
    SOURCES
        "drawline.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
