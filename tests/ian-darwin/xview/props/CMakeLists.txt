# CMake replacement for the archived make/imake build.
ian_program(ian_props
    EXECUTABLE
    SOURCES
        "props.c"
        "color_props.c"
        "color.c"
        "l10n_props.c"
        "l10n_read.c"
    DEFINITIONS
        "STANDALONE"
)
