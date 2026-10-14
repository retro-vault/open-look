# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_drawimage
    EXECUTABLE
    SOURCES
        "drawimage.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
