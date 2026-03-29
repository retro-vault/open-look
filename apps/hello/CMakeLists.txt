add_executable(hello
    main.c
)

target_link_libraries(hello
    PRIVATE
        xview
)
