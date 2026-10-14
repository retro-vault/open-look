# CMake replacement for the archived make/imake build.
ian_program(ian_guide
    LIBRARY
    SOURCES
        "src/libguide/gio.c"
        "src/libguide/gio_path.c"
        "src/libguide/guide.c"
        "src/libguide/guide_actions.c"
    DEFINITIONS
        "STANDALONE"
)
