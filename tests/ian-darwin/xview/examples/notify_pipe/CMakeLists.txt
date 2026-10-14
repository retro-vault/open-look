# CMake replacement for the archived make/imake build.
ian_program(ian_article_notify_pipe
    LIBRARY
    SOURCES
        "notify_pipe.c"
    DEFINITIONS
        "STANDALONE"
)
