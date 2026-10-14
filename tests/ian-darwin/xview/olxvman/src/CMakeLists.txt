# CMake replacement for the archived make/imake build.
ian_program(ian_olxvman
    EXECUTABLE
    SOURCES
        "globals.c"
        "man.c"
        "misc.c"
        "olxvman.c"
        "search.c"
        "olxvman_ui.c"
    LIBRARIES ian_guidexv
)
