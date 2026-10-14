# CMake replacement for the archived make/imake build.
ian_program(ian_apex_1_1
    EXECUTABLE
    SOURCES
        "alloc.c"
        "apex.c"
        "apex_help.c"
        "apex_options.c"
        "apex_shell.c"
        "editor.c"
        "file_browser.c"
        "file_io.c"
        "misc.c"
        "navigator.c"
        "object.c"
        "object_list.c"
        "s_pad.c"
        "state_server.c"
        "symbol.c"
    DEFINITIONS
        "STANDALONE"
    LIBRARIES ian_sspkg
)
