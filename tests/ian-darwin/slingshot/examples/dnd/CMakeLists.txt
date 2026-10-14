# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_drop_ex
    EXECUTABLE
    SOURCES
        "drop_ex.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_drag_ex
    EXECUTABLE
    SOURCES
        "drag_ex.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
