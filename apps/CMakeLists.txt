add_subdirectory(common)
add_subdirectory(filemgr)
add_subdirectory(clock)
add_subdirectory(cmdtool)
add_subdirectory(textedit)
add_subdirectory(props)
add_subdirectory(olwmslave)
add_subdirectory(xtoolplaces)
add_subdirectory(disktool)
add_subdirectory(vwm)
add_subdirectory(xcalc)

option(OPENLOOK_BUILD_MOSAIX "Build MosaiX (downloads CEF unless MOSAIX_CEF_ROOT is set)" ON)
if(OPENLOOK_BUILD_MOSAIX)
    add_subdirectory(mosaix)
endif()

# Install the accessories alongside the window manager.

install(TARGETS cmdtool textedit clock props xtoolplaces olwmslave disktool
    RUNTIME DESTINATION bin
)
