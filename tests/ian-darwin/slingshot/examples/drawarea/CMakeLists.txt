# CMake replacement for the archived make/imake build.
ian_program(ian_slingshot_hist
    EXECUTABLE
    SOURCES
        "hist.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_custom
    EXECUTABLE
    SOURCES
        "custom.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_hist_stdin
    EXECUTABLE
    SOURCES
        "hist_stdin.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_darea2
    EXECUTABLE
    SOURCES
        "darea2.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
ian_program(ian_slingshot_darea1
    EXECUTABLE
    SOURCES
        "darea1.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
