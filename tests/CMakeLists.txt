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

option(OPENLOOK_BUILD_IAN_DARWIN "Configure archived OpenLook CD-ROM programs" OFF)
if(OPENLOOK_BUILD_IAN_DARWIN)
    add_subdirectory(ian-darwin)
endif()

add_executable(sdk_compat_test sdk-compat-test.c)
target_link_libraries(sdk_compat_test PRIVATE xview)

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_custom_target(sdk_install_check
        COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/check-installed-sdk.py
                --build ${CMAKE_BINARY_DIR}
        DEPENDS xview xview_static olgx olgx_static
        USES_TERMINAL
        COMMENT "Checking the SDK-only install and all header dependencies"
    )
endif()
