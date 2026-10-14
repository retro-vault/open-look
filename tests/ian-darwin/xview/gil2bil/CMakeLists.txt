# CMake replacement for the archived make/imake build.
ian_program(ian_gil2bil
    EXECUTABLE
    SOURCES
        "lex.yy.c"
        "y.tab.c"
        "writeBil.c"
        "misc.c"
    DEFINITIONS
        "STANDALONE"
)
