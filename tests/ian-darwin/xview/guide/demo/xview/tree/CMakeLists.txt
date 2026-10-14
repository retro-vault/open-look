# CMake replacement for the archived make/imake build.
ian_program(ian_guide_tree
    EXECUTABLE
    SOURCES
        "tree.c"
        "tree_ui.c"
    DEFINITIONS
        "MAIN"
    LIBRARIES ian_guidexv
)
