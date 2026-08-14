file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/vroot")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/vroot.h"
    "${CMAKE_CURRENT_BINARY_DIR}/vroot/vroot.h"
    COPYONLY
)

add_executable(xtoolplaces
    xtoolplaces.c
    adddisplay.c
    addon.c
    combine.c
    copyright.c
    fix_command.c
    getargs.c
    getgeom.c
    getinfo.c
    is.c
    make_list.c
    readfile.c
    strdup.c
    strstr.c
)

target_include_directories(xtoolplaces
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(xtoolplaces
    PRIVATE
        ${X11_LIBRARIES}
)
