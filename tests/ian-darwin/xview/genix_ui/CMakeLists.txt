# CMake replacement for the archived make/imake build.
ian_program(ian_genix_ui
    EXECUTABLE
    SOURCES
        "genix_vars.c"
        "objects.c"
        "genix_ui.c"
        "genix_stubs.c"
        "objects.c"
    DEFINITIONS
        "MAIN"
        "STANDALONE"
    LIBRARIES ian_guidexv
)
set_source_files_properties(genix_vars.c objects.c genix_ui.c genix_stubs.c objects.c PROPERTIES LANGUAGE CXX)
