cmake_minimum_required(VERSION 3.16)
project(saka-note VERSION 1.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use static runtime for MSVC
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

option(LEGACY_NOTEPAD_WERROR "Treat warnings as errors" OFF)
option(SAKA_NOTE_BUILD_TESTS "Build lightweight logic tests" ON)

if(MSVC)
    add_compile_options(/W4 /WX /permissive- /utf-8)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
else()
    add_compile_options(-Wall -Wextra -municode -Wno-error=missing-field-initializers)
    if(LEGACY_NOTEPAD_WERROR)
        add_compile_options(-Werror)
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -mwindows")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    endif()
endif()

add_executable(saka-note WIN32
    src/main.cpp
    src/core/crash_diagnostics.cpp
    src/core/file_dialog_filters.cpp
    src/core/globals.cpp
    src/core/session_io.cpp
    src/core/text_codec.cpp
    src/core/versioning.cpp
    src/lang/lang.cpp
    src/modules/theme.cpp
    src/modules/editor.cpp
    src/modules/file.cpp
    src/modules/ui.cpp
    src/modules/background.cpp
    src/modules/command_routing.cpp
    src/modules/dialog.cpp
    src/modules/commands.cpp
    src/modules/icon_actions.cpp
    src/modules/perf_benchmark.cpp
    src/modules/tab_layout.cpp
    src/modules/tab_model_ops.cpp
    src/modules/tab_session_io.cpp
    src/modules/update_check.cpp
    src/modules/menu.cpp
    src/modules/settings.cpp
    src/notepad.rc
)

target_include_directories(saka-note PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/modules)
target_compile_definitions(saka-note PRIVATE UNICODE _UNICODE NOMINMAX SAKA_NOTE_SPLIT_COMMAND_MODULES=1)

target_link_libraries(saka-note PRIVATE
    comctl32
    shlwapi
    comdlg32
    shell32
    gdiplus
    msimg32
    user32
    gdi32
    kernel32
    dwmapi
    uxtheme
    winhttp
)

if(MSVC)
    # Manifest already embedded via src/notepad.rc.
    # Disable linker auto-manifest to avoid duplicate MANIFEST resource (type 1).
    target_link_options(saka-note PRIVATE /MANIFEST:NO)
endif()

if(NOT MSVC)
    target_link_options(saka-note PRIVATE -mwindows -municode)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(saka-note PRIVATE -static)
        target_link_libraries(saka-note PRIVATE -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_STRIP)
        add_custom_command(TARGET saka-note POST_BUILD COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:saka-note>)
    endif()
endif()

if(SAKA_NOTE_BUILD_TESTS)
    enable_testing()
    add_executable(saka-note-versioning-tests
        tests/versioning_tests.cpp
        src/core/versioning.cpp
    )
    target_include_directories(saka-note-versioning-tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(saka-note-versioning-tests PRIVATE UNICODE _UNICODE NOMINMAX)
    add_test(NAME saka-note-versioning-tests COMMAND saka-note-versioning-tests)

    add_executable(saka-note-encoding-tests
        tests/encoding_tests.cpp
        src/core/text_codec.cpp
    )
    target_include_directories(saka-note-encoding-tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(saka-note-encoding-tests PRIVATE UNICODE _UNICODE NOMINMAX)
    add_test(NAME saka-note-encoding-tests COMMAND saka-note-encoding-tests)

    add_executable(saka-note-session-io-tests
        tests/session_io_tests.cpp
        src/core/session_io.cpp
    )
    target_include_directories(saka-note-session-io-tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(saka-note-session-io-tests PRIVATE UNICODE _UNICODE NOMINMAX)
    add_test(NAME saka-note-session-io-tests COMMAND saka-note-session-io-tests)

    add_executable(saka-note-tab-session-io-tests
        tests/tab_session_io_tests.cpp
        src/core/session_io.cpp
        src/core/text_codec.cpp
        src/modules/tab_session_io.cpp
    )
    target_include_directories(saka-note-tab-session-io-tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/modules)
    target_compile_definitions(saka-note-tab-session-io-tests PRIVATE UNICODE _UNICODE NOMINMAX)
    target_link_libraries(saka-note-tab-session-io-tests PRIVATE shlwapi)
    add_test(NAME saka-note-tab-session-io-tests COMMAND saka-note-tab-session-io-tests)

    add_executable(saka-note-tab-model-ops-tests
        tests/tab_model_ops_tests.cpp
        src/modules/tab_model_ops.cpp
    )
    target_include_directories(saka-note-tab-model-ops-tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/modules)
    target_compile_definitions(saka-note-tab-model-ops-tests PRIVATE UNICODE _UNICODE NOMINMAX)
    add_test(NAME saka-note-tab-model-ops-tests COMMAND saka-note-tab-model-ops-tests)
endif()

# Installation
install(TARGETS saka-note DESTINATION .)
install(FILES LICENSE README.md DESTINATION .)

# Installer / packaging (CPack with NSIS)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "saka-note")
set(CPACK_PACKAGE_VENDOR "Saka Studio & Engineering")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
if(CMAKE_GENERATOR_PLATFORM)
    set(CPACK_PACKAGE_FILE_NAME "saka-note-${PROJECT_VERSION}-${CMAKE_GENERATOR_PLATFORM}")
else()
    set(CPACK_PACKAGE_FILE_NAME "saka-note-${PROJECT_VERSION}")
endif()
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_NSIS_DISPLAY_NAME "Saka Note")
set(CPACK_NSIS_PACKAGE_NAME "Saka Note")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/Karta-Sena/Saka-Note")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/src/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/src/icon.ico")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_PACKAGE_EXECUTABLES "saka-note.exe" "Saka Note")
set(CPACK_NSIS_INSTALLED_ICON_NAME "saka-note.exe")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "saka-note.exe")
set(CPACK_NSIS_MENU_LINKS
    "saka-note.exe" "Saka Note"
)

include(CPack)
