# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_tlstool
    EXECUTABLE
    SOURCES
        "tlstool.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
