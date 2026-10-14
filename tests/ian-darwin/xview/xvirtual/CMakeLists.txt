# CMake replacement for the archived make/imake build.
ian_program(ian_xvirtual
    EXECUTABLE
    SOURCES
        "animate.cc"
        "clipping.cc"
        "figuren.cc"
        "flaeche.cc"
        "koerper.cc"
        "load.cc"
        "main.cc"
        "nvekt.cc"
        "shape.cc"
        "szene.cc"
        "view.cc"
        "x.cc"
        "zbuffer.cc"
        "shadow.cc"
    DEFINITIONS
        "STANDALONE"
)
set_target_properties(ian_xvirtual PROPERTIES CXX_STANDARD 98 CXX_STANDARD_REQUIRED YES)
