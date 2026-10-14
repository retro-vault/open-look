# CMake replacement for the archived make/imake build.
ian_program(ian_timekeeper
    EXECUTABLE
    SOURCES
        "topics.c"
        "logfile.c"
        "timer_ui.c"
        "timer_stubs.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
ian_program(ian_tkrefmt
    EXECUTABLE
    SOURCES
        "tkrefmt.c"
)
ian_program(ian_timekeeper_alternate
    EXECUTABLE
    SOURCES
        "timer.c"
        "timer_ui.c"
        "topics.c"
        "logfile.c"
    DEFINITIONS
        "MAIN"
    LIBRARIES ian_guidexv
)
