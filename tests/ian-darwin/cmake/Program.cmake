# Keep each archived program independently buildable and namespaced.
function(ian_program name)
    cmake_parse_arguments(APP "EXECUTABLE;LIBRARY" "" "SOURCES;DEFINITIONS;LIBRARIES" ${ARGN})
    if(NOT APP_SOURCES)
        add_custom_target(${name}
            COMMAND ${CMAKE_COMMAND} -E echo "${name}: upstream has no program sources"
            COMMAND ${CMAKE_COMMAND} -E false
            VERBATIM)
        return()
    endif()
    foreach(source IN LISTS APP_SOURCES)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
            # Generated lexer/parser source is handled separately below.
            if(NOT (name STREQUAL "ian_Ftptool4_6b4" AND source STREQUAL "lex.c")
               AND NOT (name STREQUAL "ian_xvnews_2_3" AND source STREQUAL "getdate.c"))
                add_custom_target(${name}
                    COMMAND ${CMAKE_COMMAND} -E echo "${name}: missing upstream source ${source}"
                    COMMAND ${CMAKE_COMMAND} -E false VERBATIM)
                return()
            endif()
        endif()
    endforeach()
    if(name STREQUAL "ian_Ftptool4_6b4")
        find_package(FLEX REQUIRED)
        flex_target(ian_ftp_lexer lex.l "${CMAKE_CURRENT_BINARY_DIR}/lex.c")
        list(REMOVE_ITEM APP_SOURCES lex.c)
        list(APPEND APP_SOURCES ${FLEX_ian_ftp_lexer_OUTPUTS})
    elseif(name STREQUAL "ian_xvnews_2_3")
        find_package(BISON REQUIRED)
        bison_target(ian_news_parser getdate.y "${CMAKE_CURRENT_BINARY_DIR}/getdate.c")
        list(REMOVE_ITEM APP_SOURCES getdate.c)
        list(APPEND APP_SOURCES ${BISON_ian_news_parser_OUTPUTS})
    elseif(name STREQUAL "ian_gil2bil")
        find_package(FLEX REQUIRED)
        find_package(BISON REQUIRED)
        bison_target(ian_gil_parser gil.y
            "${CMAKE_CURRENT_BINARY_DIR}/gil-parser.c"
            DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/gil-parser.h")
        flex_target(ian_gil_lexer gil.l
            "${CMAKE_CURRENT_BINARY_DIR}/gil-lexer.c")
        add_flex_bison_dependency(ian_gil_lexer ian_gil_parser)
        list(REMOVE_ITEM APP_SOURCES lex.yy.c y.tab.c)
        list(APPEND APP_SOURCES ${BISON_ian_gil_parser_OUTPUTS}
            ${FLEX_ian_gil_lexer_OUTPUTS})
    elseif(name STREQUAL "ian_xplan")
        find_package(FLEX REQUIRED)
        find_package(BISON REQUIRED)
        bison_target(ian_plan_parser parser.y
            "${CMAKE_CURRENT_BINARY_DIR}/xplan-parser.c"
            DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/xplan-parser.h")
        flex_target(ian_plan_lexer scanner.l
            "${CMAKE_CURRENT_BINARY_DIR}/xplan-scanner.c")
        add_flex_bison_dependency(ian_plan_lexer ian_plan_parser)
        list(REMOVE_ITEM APP_SOURCES parser.c scanner.c)
        list(APPEND APP_SOURCES ${BISON_ian_plan_parser_OUTPUTS}
            ${FLEX_ian_plan_lexer_OUTPUTS})
    elseif(name STREQUAL "ian_xvtdl_5_0" OR name STREQUAL "ian_tdl")
        find_package(FLEX REQUIRED)
        find_package(BISON REQUIRED)
        set(parser_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/${name}")
        file(MAKE_DIRECTORY "${parser_dir}")
        flex_target(${name}_lexer todo.l "${parser_dir}/todo-lexer.c")
        bison_target(${name}_parser todo.y "${parser_dir}/todo-parser.c")
        # todo.y includes the scanner, sharing its grammar token definitions.
        add_flex_bison_dependency(${name}_lexer ${name}_parser)
        set_source_files_properties(${FLEX_${name}_lexer_OUTPUTS}
            PROPERTIES HEADER_FILE_ONLY TRUE)
        list(REMOVE_ITEM APP_SOURCES lex.yy.c y.tab.c)
        list(APPEND APP_SOURCES ${BISON_${name}_parser_OUTPUTS}
            ${FLEX_${name}_lexer_OUTPUTS})
    endif()
    if(APP_LIBRARY)
        add_library(${name} STATIC EXCLUDE_FROM_ALL ${APP_SOURCES})
    else()
        add_executable(${name} EXCLUDE_FROM_ALL ${APP_SOURCES})
    endif()
    target_link_libraries(${name} PRIVATE xview m ${APP_LIBRARIES})
    target_compile_definitions(${name} PRIVATE ${APP_DEFINITIONS})
    # FSPtool has a local xview/ directory; it must not shadow SDK headers.
    target_include_directories(${name} BEFORE PRIVATE
        "${PROJECT_SOURCE_DIR}/include")
    target_include_directories(${name} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
        "${IAN_ROOT}/xview/guide/include"
        "${IAN_ROOT}/xview/guide/src/libguide"
        "${IAN_ROOT}/xview/guide/src/libguidexv"
        "${IAN_ROOT}/slingshot/include")
    if(name STREQUAL "ian_dstool")
        target_include_directories(${name} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/models")
    endif()
    set_target_properties(${name} PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES
        RUNTIME_OUTPUT_DIRECTORY "${OPENWINHOME_STAGE_BIN_DIR}/ian-darwin"
        ARCHIVE_OUTPUT_DIRECTORY "${OPENWINHOME_STAGE_LIB_DIR}/ian-darwin")
    foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(${name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${config} "${OPENWINHOME_STAGE_BIN_DIR}/ian-darwin"
            ARCHIVE_OUTPUT_DIRECTORY_${config} "${OPENWINHOME_STAGE_LIB_DIR}/ian-darwin")
    endforeach()
endfunction()
