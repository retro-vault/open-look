# CMake replacement for the archived make/imake build.
ian_program(ian_oldemo
    EXECUTABLE
    SOURCES
        "oldemo_ui.c"
        "oldemo_stubs.c"
    DEFINITIONS
        "MAIN"
    LIBRARIES ian_guidexv
)
