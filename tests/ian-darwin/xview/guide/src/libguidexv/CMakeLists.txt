# CMake replacement for the archived make/imake build.
ian_program(ian_guidexv
    LIBRARY
    SOURCES
        "gcc.c"
        "gcc_stubs.c"
        "gcc_ui.c"
        "gcm.c"
        "gdd.c"
        "gfm.c"
        "gfm_load_dir.c"
        "gfm_stubs.c"
        "gfm_ui.c"
        "group.c"
        "group_data.c"
        "gutil.c"
    LIBRARIES ian_guide
)
