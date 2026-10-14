# CMake replacement for the archived make/imake build.
ian_program(ian_videoteXt_0_5
    EXECUTABLE
    SOURCES
        "cct.c"
        "vtxdecode.c"
        "vtxtools.c"
        "toptext.c"
        "safe_malloc.c"
        "postscript.c"
    DEFINITIONS
        "STANDALONE"
)
