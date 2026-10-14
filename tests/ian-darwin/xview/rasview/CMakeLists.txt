# CMake replacement for the archived make/imake build.
ian_program(ian_rasview
    EXECUTABLE
    SOURCES
        "rasview.c"
        "rasview_ui.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
