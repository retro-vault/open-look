# resources/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

set(OPENWINHOME_RESOURCE_INCLUDE_DIR "${OPENWINHOME_STAGE_INCLUDE_DIR}")
set(OPENWINHOME_RESOURCE_LIB_DIR "${OPENWINHOME_STAGE_LIB_DIR}")
set(OPENWINHOME_RESOURCE_SHARE_DIR "${OPENWINHOME_STAGE_SHARE_DIR}")
set(OPENWINHOME_MAN1_DIR "${OPENWINHOME_STAGE_SHARE_DIR}/man/man1")
set(OPENWINHOME_FONT_DIR "${OPENWINHOME_STAGE_SHARE_DIR}/fonts/openlook")
set(OPENLOOK_MAN_PAGES
    clock
    olwm
    olwmslave
    props
    textedit
    xtoolplaces
)

set(OPENLOOK_MAN_STAGE_COMMANDS)
foreach(man_page IN LISTS OPENLOOK_MAN_PAGES)
    list(APPEND OPENLOOK_MAN_STAGE_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${PROJECT_SOURCE_DIR}/docs/man/${man_page}.man"
                "${OPENWINHOME_MAN1_DIR}/${man_page}.1"
    )
endforeach()

# Stage runtime resources into the OPENWINHOME tree.
add_custom_target(openlook_resources ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENWINHOME_RESOURCE_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENWINHOME_RESOURCE_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENWINHOME_MAN1_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENWINHOME_FONT_DIR}"

    # Runtime include assets expected by legacy XView/OpenWindows code.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/images"
            "${OPENWINHOME_RESOURCE_INCLUDE_DIR}/images"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/bitmaps"
            "${OPENWINHOME_RESOURCE_INCLUDE_DIR}/bitmaps"
    # Compatibility path used by olwm bitmap lookup.
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${OPENWINHOME_RESOURCE_INCLUDE_DIR}/X11"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/bitmaps"
            "${OPENWINHOME_RESOURCE_INCLUDE_DIR}/X11/bitmaps"

    # Menu/config defaults looked up under $OPENWINHOME/lib.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/config/openwin-menu"
            "${OPENWINHOME_RESOURCE_LIB_DIR}/openwin-menu"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/config/openwin-menu-programs"
            "${OPENWINHOME_RESOURCE_LIB_DIR}/openwin-menu-programs"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/config/openwin-menu-demo"
            "${OPENWINHOME_RESOURCE_LIB_DIR}/openwin-menu-demo"
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${OPENWINHOME_RESOURCE_LIB_DIR}/app-defaults"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/config/Xdefaults"
            "${OPENWINHOME_RESOURCE_LIB_DIR}/app-defaults/Olwm"

    # Manual pages in standard share/man hierarchy.
    ${OPENLOOK_MAN_STAGE_COMMANDS}

    # Fonts are staged under share; on a real install they should be added
    # to the system X font path.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
            "${OPENWINHOME_FONT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/locale"
            "${OPENWINHOME_RESOURCE_SHARE_DIR}/locale"
    COMMENT "Staging OpenLook resources into ${OPENWINHOME_STAGE_DIR}"
)

# Keep install layout aligned with staged runtime layout.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bitmaps"
    DESTINATION include
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/images"
    DESTINATION include
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    DESTINATION share/fonts/openlook
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/locale/"
    DESTINATION share/locale
)
install(FILES "${PROJECT_SOURCE_DIR}/config/openwin-menu"
              "${PROJECT_SOURCE_DIR}/config/openwin-menu-programs"
              "${PROJECT_SOURCE_DIR}/config/openwin-menu-demo"
    DESTINATION lib
)
install(FILES "${PROJECT_SOURCE_DIR}/config/Xdefaults"
    DESTINATION lib/app-defaults
    RENAME Olwm
)
foreach(man_page IN LISTS OPENLOOK_MAN_PAGES)
    install(FILES "${PROJECT_SOURCE_DIR}/docs/man/${man_page}.man"
        DESTINATION share/man/man1
        RENAME "${man_page}.1"
    )
endforeach()
