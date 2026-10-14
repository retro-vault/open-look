# CMake replacement for the archived make/imake build.
ian_program(ian_clock
    EXECUTABLE
    SOURCES
        "clock.c"
    DEFINITIONS
        "STANDALONE"
)
target_sources(ian_clock PRIVATE
    "${PROJECT_SOURCE_DIR}/apps/clock/ds_popup.c"
)
