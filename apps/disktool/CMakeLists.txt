add_executable(disktool
    disktool.c
)

target_link_libraries(disktool
    PRIVATE
        app_icons
)

install(FILES COPYRIGHT DESTINATION share/doc/openlook/disktool)
