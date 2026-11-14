add_executable(props_color_test
    props-color-test.c
    props-entry.c
    ${PROJECT_SOURCE_DIR}/apps/props/color.c
    ${PROJECT_SOURCE_DIR}/apps/props/l10n_props.c
    ${PROJECT_SOURCE_DIR}/apps/props/l10n_read.c
)
target_link_libraries(props_color_test PRIVATE xview m)
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
    ${PROJECT_SOURCE_DIR}/apps/clock/ds_popup.c
)
target_compile_definitions(clock_preferences_test PRIVATE _DEFAULT_SOURCE)
target_link_libraries(clock_preferences_test PRIVATE xview m)
set_target_properties(clock_preferences_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(clock_preferences_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()
