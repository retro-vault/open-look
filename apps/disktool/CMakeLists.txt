add_executable(disktool
    disktool.c
)

target_link_libraries(disktool
    PRIVATE
        xview
)
