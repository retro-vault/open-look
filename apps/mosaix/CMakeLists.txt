# Same verified CEF distribution as Gemscape; native X11/XWayland embedding.
set(MOSAIX_CEF_ROOT "" CACHE PATH "Existing extracted CEF 154 minimal distribution")
set(MOSAIX_CEF_VERSION "154.0.23+g062ebe4+chromium-154.0.8037.17")
if(NOT MOSAIX_CEF_ROOT)
    set(MOSAIX_CEF_ROOT "${CMAKE_BINARY_DIR}/deps/cef/source")
    set(mosaix_cef_managed TRUE)
endif()

# Reuse a complete, matching distribution already extracted in the build tree.
# This also supports checkouts prepared before ExternalProject stamp files exist.
set(mosaix_cef_ready TRUE)
foreach(required include/cef_version.h include/capi/cef_app_capi.h
        Release/libcef.so Resources/icudtl.dat Resources/locales LICENSE.txt)
    if(NOT EXISTS "${MOSAIX_CEF_ROOT}/${required}")
        set(mosaix_cef_ready FALSE)
    endif()
endforeach()
if(mosaix_cef_ready)
    file(STRINGS "${MOSAIX_CEF_ROOT}/include/cef_version.h" mosaix_cef_version_line
        REGEX "^#define CEF_VERSION ")
    if(NOT mosaix_cef_version_line STREQUAL "#define CEF_VERSION \"${MOSAIX_CEF_VERSION}\"")
        set(mosaix_cef_ready FALSE)
    endif()
endif()
if(NOT mosaix_cef_ready AND NOT mosaix_cef_managed)
    message(FATAL_ERROR
        "MOSAIX_CEF_ROOT='${MOSAIX_CEF_ROOT}' must contain an extracted CEF "
        "${MOSAIX_CEF_VERSION} minimal distribution. "
        "Configure with -DMOSAIX_CEF_ROOT= to use the managed download.")
endif()
if(NOT mosaix_cef_ready)
    include(ExternalProject)
    ExternalProject_Add(mosaix_cef_download
        URL "https://cef-builds.spotifycdn.com/cef_binary_${MOSAIX_CEF_VERSION}_linux64_minimal.tar.bz2"
        URL_HASH SHA256=8beed3410b79aa4ae964bb7f955f08ac550eb2a377a6d12090be8bdb7de1369d
        TLS_VERIFY TRUE
        PREFIX "${CMAKE_BINARY_DIR}/deps/cef"
        DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/deps/downloads"
        SOURCE_DIR "${MOSAIX_CEF_ROOT}"
        CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND ""
        BUILD_BYPRODUCTS "${MOSAIX_CEF_ROOT}/Release/libcef.so")
endif()
set(cef_stage "${OPENWINHOME_STAGE_LIB_DIR}/mosaix/cef")
add_custom_target(mosaix_cef_stage
    COMMAND ${CMAKE_COMMAND} -E make_directory "${cef_stage}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MOSAIX_CEF_ROOT}/Release" "${cef_stage}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MOSAIX_CEF_ROOT}/Resources" "${cef_stage}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MOSAIX_CEF_ROOT}/LICENSE.txt" "${cef_stage}/LICENSE.txt"
    COMMENT "Staging MosaiX private CEF runtime")
if(TARGET mosaix_cef_download)
    add_dependencies(mosaix_cef_stage mosaix_cef_download)
endif()
add_library(mosaix_cef SHARED IMPORTED)
set_target_properties(mosaix_cef PROPERTIES IMPORTED_LOCATION "${MOSAIX_CEF_ROOT}/Release/libcef.so")
add_executable(mosaix mosaix.c browser.c preferences.c toolbar-icons.c)
add_dependencies(mosaix mosaix_cef_stage)
target_include_directories(mosaix PRIVATE "${MOSAIX_CEF_ROOT}")
# CEF/NSS worker threads need libc I/O, not XView's process-global notifier
# interposers (notably fcntl64, whose legacy argument handling truncates pointers).
# CEF itself must precede libc so its RTLD_NEXT lookups still find libc.
if(NOT X11_Xrender_FOUND)
    message(FATAL_ERROR "MosaiX transparent icons require libXrender development files")
endif()
target_link_libraries(mosaix PRIVATE mosaix_cef c app_icons m ${X11_Xrender_LIB})
target_compile_features(mosaix PRIVATE c_std_11)
target_compile_options(mosaix PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter)
set_target_properties(mosaix PROPERTIES
    BUILD_RPATH "${cef_stage};${OPENWINHOME_STAGE_LIB_DIR}"
    INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/../lib/mosaix/cef")
install(TARGETS mosaix RUNTIME DESTINATION bin)
install(FILES icons/COPYING icons/UPSTREAM.txt DESTINATION share/doc/mosaix/icons)
install(DIRECTORY "${cef_stage}/" DESTINATION lib/mosaix/cef USE_SOURCE_PERMISSIONS)
