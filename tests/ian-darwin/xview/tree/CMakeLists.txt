# CMake replacement for the archived make/imake build.
ian_program(ian_tree
    EXECUTABLE
    SOURCES
        "tree.c"
        "tree_ui.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
ian_program(ian_tree_generated
    EXECUTABLE
    SOURCES
        "tree_stubs.c"
        "tree_ui.c"
    DEFINITIONS
        "MAIN"
    LIBRARIES ian_guidexv
)
