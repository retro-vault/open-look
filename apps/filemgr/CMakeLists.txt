# Build the original DeskSet File Manager against this tree's Linux XView.
add_executable(filemgr
    assoc.c display.c dragdrop.c file.c filemgr_ui.c functions.c isubs.c
    main.c text.c various.c xview.c linux_port.c
    support/group.c support/group_data.c support/gutil.c
    support/ds_colors.c support/ds_popup.c support/ds_xlib.c
    support/ds_pathname.c support/ds_relname.c support/ds_template.c
    support/ds_item.c
)
target_compile_definitions(filemgr PRIVATE SVR4 _DEFAULT_SOURCE)
target_include_directories(filemgr PRIVATE support ${PROJECT_SOURCE_DIR}/resources)
target_compile_options(filemgr PRIVATE
    -include ${CMAKE_CURRENT_SOURCE_DIR}/linux_port.h
    -Werror=implicit-function-declaration -Werror=implicit-int
    -Werror=incompatible-pointer-types -Werror=int-conversion
)
find_package(X11 REQUIRED COMPONENTS Xpm)
target_link_libraries(filemgr PRIVATE xview ${X11_Xpm_LIB} m)
