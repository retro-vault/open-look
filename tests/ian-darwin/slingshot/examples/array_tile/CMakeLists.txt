# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_array_ex
    EXECUTABLE
    SOURCES
        "array_ex.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
