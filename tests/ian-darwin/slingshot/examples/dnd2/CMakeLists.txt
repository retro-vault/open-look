# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_cei
    EXECUTABLE
    SOURCES
        "cei.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_icon_dnd
    EXECUTABLE
    SOURCES
        "icon_dnd.c"
        "cei.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
