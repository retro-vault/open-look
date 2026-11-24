add_executable(cmdtool
    cmdtool.c
)

target_link_libraries(cmdtool
    PRIVATE
        app_icons
)

# cmdtool selects terminal mode from argv[0], so shelltool shares its binary.
add_custom_target(shelltool ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            cmdtool "${OPENWINHOME_STAGE_BIN_DIR}/shelltool"
    DEPENDS cmdtool
    COMMENT "Staging the shelltool symlink"
    VERBATIM
)

# Installing the relative link preserves relocation and DESTDIR support.
install(PROGRAMS "${OPENWINHOME_STAGE_BIN_DIR}/shelltool"
    DESTINATION bin
)
