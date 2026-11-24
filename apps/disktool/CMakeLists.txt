add_executable(disktool
    disktool.c
)

target_link_libraries(disktool
    PRIVATE
        xview
)

install(FILES COPYRIGHT DESTINATION share/doc/openlook/disktool)
