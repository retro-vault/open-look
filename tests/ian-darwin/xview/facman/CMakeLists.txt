# CMake replacement for the archived make/imake build.
ian_program(ian_facman
    EXECUTABLE
    SOURCES
        "actions.c"
        "facman_stubs.c"
        "facman_ui.c"
    DEFINITIONS
        "MAIN"
    LIBRARIES ian_guidexv
)
