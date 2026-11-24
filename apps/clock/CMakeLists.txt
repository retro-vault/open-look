add_executable(clock
    clock.c
    clock-face.c
    ds_popup.c
)

target_link_libraries(clock
    PRIVATE
        app_icons
        m
)
