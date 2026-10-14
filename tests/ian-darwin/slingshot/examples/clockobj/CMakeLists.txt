# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_clockobj
    EXECUTABLE
    SOURCES
        "clockobj.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
