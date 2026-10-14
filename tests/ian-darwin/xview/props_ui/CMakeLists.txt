# CMake replacement for the archived make/imake build.
ian_program(ian_props_ui
    EXECUTABLE
    SOURCES
        "props_ui.c"
        "props_stubs.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
