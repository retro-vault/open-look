# CMake replacement for the archived make/imake build.
ian_program(ian_example_selection_sel_hold
    EXECUTABLE
    SOURCES
        "sel_hold.c"
    DEFINITIONS
        "STANDALONE"
)
ian_program(ian_example_selection_sel_req
    EXECUTABLE
    SOURCES
        "sel_req.c"
    DEFINITIONS
        "STANDALONE"
)
