# src/olwm/CMakeLists.txt

# List of source files
set(OLWM_SOURCES
    atom.c client.c cmdstream.c debug.c defaults.c
    dsdm.c environ.c error.c evbind.c events.c group.c
    helpsend.c i18n.c info.c kbdfuncs.c list.c mem.c
    menu.c moveresize.c olwm.c notice.c properties.c
    resources.c screen.c services.c selection.c
    slave.c slots.c st.c states.c usermenu.c usleep.c
    win.c winbusy.c winbutton.c wincolor.c winframe.c
    wingframe.c winicon.c winipane.c winmenu.c
    winnofoc.c winpane.c winpinmenu.c winpush.c
    winresize.c winroot.c fallback_allplanes.c
)

# Defines
add_compile_definitions(
    ALLPLANES
    SHAPE
)

# Optionally enable debug/profile via options
option(OLWM_ENABLE_DEBUG "Enable debug mode" OFF)
option(OLWM_ENABLE_GPROF "Enable gprof profiling" OFF)

if(OLWM_ENABLE_DEBUG)
    add_compile_definitions(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()

if(OLWM_ENABLE_GPROF)
    add_compile_definitions(GPROF_HOOKS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -Bstatic")
endif()

# Executable
add_executable(olwm ${OLWM_SOURCES})

# Temporarily disable compiler warnings while porting
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(olwm PRIVATE -w)
endif()

# Includes: local + possibly OPENWINHOME (if emulated)
target_include_directories(olwm
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against olgx and X11
target_link_libraries(olwm
    PRIVATE
        olgx
        ${X11_LIBRARIES}
        ${X11_Xext_LIB}  # optional but probably needed
)

# Optional: install target
install(TARGETS olwm RUNTIME DESTINATION bin)
