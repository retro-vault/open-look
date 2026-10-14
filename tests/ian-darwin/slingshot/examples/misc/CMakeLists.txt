# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_drawrect
    EXECUTABLE
    SOURCES
        "drawrect.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_example1
    EXECUTABLE
    SOURCES
        "example1.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
