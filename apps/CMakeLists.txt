add_subdirectory(filemgr)
add_subdirectory(clock)
add_subdirectory(cmdtool)
add_subdirectory(textedit)
add_subdirectory(props)
add_subdirectory(olwmslave)
add_subdirectory(xtoolplaces)

# Install the accessories alongside the window manager.
install(TARGETS filemgr cmdtool textedit clock props xtoolplaces olwmslave
    RUNTIME DESTINATION bin
)
