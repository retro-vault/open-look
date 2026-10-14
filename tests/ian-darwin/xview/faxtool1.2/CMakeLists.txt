# CMake replacement for the archived make/imake build.
ian_program(ian_faxtool1_2
    EXECUTABLE
    SOURCES
        "faxtool.c"
        "faxtool_ui.c"
        "faxtool_stubs.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
