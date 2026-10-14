# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_puzzdemo
    EXECUTABLE
    SOURCES
        "puzzdemo.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_nonlin
    EXECUTABLE
    SOURCES
        "nonlin.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_temp_grip
    EXECUTABLE
    SOURCES
        "temp_grip.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_gripdemo
    EXECUTABLE
    SOURCES
        "gripdemo.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
