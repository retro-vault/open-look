# CMake replacement for the archived make/imake build.
ian_program(ian_sidtool
    EXECUTABLE
    SOURCES
        "action.c"
        "main.c"
        "setup.c"
        "sidtool_ui.c"
        "xview.c"
    DEFINITIONS
        "STANDALONE"
)
