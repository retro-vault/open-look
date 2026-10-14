# CMake replacement for the archived make/imake build.
ian_program(ian_example_icons_icon_demo
    EXECUTABLE
    SOURCES
        "icon_demo.c"
    DEFINITIONS
        "STANDALONE"
)
ian_program(ian_example_icons_icon_demo2
    EXECUTABLE
    SOURCES
        "icon_demo2.c"
    DEFINITIONS
        "STANDALONE"
)
