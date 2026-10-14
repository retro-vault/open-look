# CMake replacement for the archived make/imake build.
ian_program(ian_NADagent
    EXECUTABLE
    SOURCES
        "main.c"
        "panel.c"
        "cards.c"
        "phone.c"
        "soundex.c"
    DEFINITIONS
        "STANDALONE"
)
