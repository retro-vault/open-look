add_executable(props_color_test
    props-color-test.c
    props-entry.c
    ${PROJECT_SOURCE_DIR}/apps/props/color.c
    ${PROJECT_SOURCE_DIR}/apps/props/l10n_props.c
    ${PROJECT_SOURCE_DIR}/apps/props/l10n_read.c
)
target_link_libraries(props_color_test PRIVATE app_icons m)
set_target_properties(props_color_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(props_color_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

# Includes the application to exercise its real private preferences routines.
add_executable(clock_preferences_test EXCLUDE_FROM_ALL
    clock-preferences-test.c
    ${PROJECT_SOURCE_DIR}/apps/clock/clock-face.c
    ${PROJECT_SOURCE_DIR}/apps/clock/ds_popup.c
)
target_compile_definitions(clock_preferences_test PRIVATE _DEFAULT_SOURCE)
target_link_libraries(clock_preferences_test PRIVATE app_icons m)
set_target_properties(clock_preferences_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(clock_preferences_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

# Exercises clock geometry and actual dial pixels at fixed times.
add_executable(clock_face_test EXCLUDE_FROM_ALL clock-face-test.c
    ${PROJECT_SOURCE_DIR}/apps/clock/clock-face.c)
target_link_libraries(clock_face_test PRIVATE app_icons m)
target_compile_options(clock_face_test PRIVATE -Wall -Wextra -Werror)
set_target_properties(clock_face_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(clock_face_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}")
endforeach()

# Exercises the public fit APIs against real XView panel geometry.
add_executable(panel_fit_test EXCLUDE_FROM_ALL panel-fit-test.c)
target_link_libraries(panel_fit_test PRIVATE xview)
target_compile_options(panel_fit_test PRIVATE -Wall -Wextra -Werror)
set_target_properties(panel_fit_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(panel_fit_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

add_executable(textsw_close_test EXCLUDE_FROM_ALL textsw-close-test.c)
target_link_libraries(textsw_close_test PRIVATE xview)
target_compile_options(textsw_close_test PRIVATE -Wall -Wextra -Werror)
set_target_properties(textsw_close_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(textsw_close_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}")
endforeach()
