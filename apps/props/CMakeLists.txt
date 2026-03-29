add_executable(props
    props.c
    color.c
    color_props.c
    l10n_props.c
    l10n_read.c
)

target_link_libraries(props
    PRIVATE
        xview
)
