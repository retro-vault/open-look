add_executable(cmdtool
    cmdtool.c
)

target_link_libraries(cmdtool
    PRIVATE
        xview
)
