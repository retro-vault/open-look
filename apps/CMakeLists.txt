add_subdirectory(hello)
add_subdirectory(clock)
add_subdirectory(cmdtool)
add_subdirectory(textedit)
add_subdirectory(props)
add_subdirectory(olwmslave)
add_subdirectory(xtoolplaces)
add_subdirectory(disktool)

# Install the accessories alongside the window manager.
install(TARGETS cmdtool textedit clock props xtoolplaces hello olwmslave disktool
    RUNTIME DESTINATION bin
)
