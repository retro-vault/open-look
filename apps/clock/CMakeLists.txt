add_executable(clock
    clock.c
    ds_popup.c
)

target_link_libraries(clock
    PRIVATE
        xview
        m
)
