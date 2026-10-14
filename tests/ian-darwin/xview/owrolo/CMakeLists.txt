# CMake replacement for the archived make/imake build.
ian_program(ian_owrolo
    EXECUTABLE
    SOURCES
        "main.c"
        "cards.c"
        "popup.c"
        "panel.c"
        "openlook.c"
    DEFINITIONS
        "STANDALONE"
)
