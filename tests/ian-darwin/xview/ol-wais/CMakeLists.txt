# CMake replacement for the archived make/imake build.
ian_program(ian_ol_wais
    EXECUTABLE
    SOURCES
        "olwais_ui.cc"
        "olwais_stubs.cc"
        "olutils.cc"
        "directory.cc"
    DEFINITIONS
        "MAIN"
    LIBRARIES ian_guidexv Xpm
)
