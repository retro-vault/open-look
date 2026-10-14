# CMake replacement for the archived make/imake build.
ian_program(ian_yamailt_ui
    EXECUTABLE
    SOURCES
        "yamailt_ui.c"
        "yamailt_stubs.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
