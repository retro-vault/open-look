add_executable(textedit
    textedit.c
)

target_link_libraries(textedit
    PRIVATE
        xview
)

target_compile_definitions(textedit
    PRIVATE
        STANDALONE
)

target_compile_options(textedit
    PRIVATE
        -Wno-deprecated-declarations
)
