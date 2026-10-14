# CMake replacement for the archived make/imake build.
ian_program(ian_example_defaults_default_size
    EXECUTABLE
    SOURCES
        "default_size.c"
    DEFINITIONS
        "STANDALONE"
)
ian_program(ian_example_defaults_default_text
    EXECUTABLE
    SOURCES
        "default_text.c"
    DEFINITIONS
        "STANDALONE"
)
