add_executable(olwmslave
    olwmslave.c
    cmdstream.c
    help_file.c
    helprecv.c
    helpwin.c
    i18n.c
    mem.c
    propsrecv.c
    propssend.c
    propswin.c
)

target_compile_definitions(olwmslave
    PRIVATE
        OLWMSLAVE
        OW_I18N_L3
)

target_link_libraries(olwmslave
    PRIVATE
        xview
)
