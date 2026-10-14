# CMake replacement for the archived make/imake build.
ian_program(ian_privtool_0_83
    EXECUTABLE
    SOURCES
        "buffers.c"
        "gui.c"
        "mail_reader.c"
        "main.c"
        "messages.c"
        "pgplib.c"
        "x.c"
    DEFINITIONS
        "STANDALONE"
)
