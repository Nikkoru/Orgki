cmake_minimum_required(VERSION 3.5)

# Enable Hot Reload for MSVC compilers if supported.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
endif()

project("Orgki")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(ORGKI_HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")

option(ORGKI_CLI "builds a CLI instead of TUI" OFF)
option(ORGKI_GUI "Builds a GUI app instead of TUI" ON)

option(ORGKI_STATIC "Builds, with static linking instead of dynamic linking dependencies" ON)

add_subdirectory(externals)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)

add_executable(Orgki
    ${Orgki_SOURCES}
    ${Orgki_HEADERS}
)

if (ORGKI_GUI)
    if (WIN32)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            list(APPEND CMAKE_PREFIX_PATH $ENV{QT_MSVC})
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            list(APPEND CMAKE_PREFIX_PATH $ENV{QT_MINGW})
        endif()
    endif()
    find_package(Qt6 COMPONENTS Gui Widgets REQUIRED)
    qt_standard_project_setup()
endif()

set_property(TARGET Orgki PROPERTY CXX_STANDARD 23)
target_compile_features(Orgki PRIVATE cxx_std_23)
set_target_properties(Orgki PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if (ORGKI_CLI)
    target_compile_definitions(Orgki PRIVATE ORGKI_CLI)
elseif(ORGKI_GUI)
    target_compile_definitions(Orgki PRIVATE ORGKI_GUI)
endif()

if (ORGKI_STATIC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options("-static")
        add_compile_options("$<$<COMPILE_LANGUAGE::CXX>:-static-libstdc++>")
        add_compile_options("$<$<COMPILE_LANGUAGE::CXX>:-static-libgcc>")
        add_compile_options("$<$<COMPILE_LANGUAGE::C>:-static-libgcc>")
    endif()
endif()

if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
else()
    target_link_libraries(Orgki
        PRIVATE
            stdc++exp
    )
endif()

target_link_libraries(Orgki
    PRIVATE
        ftxui::ftxui
        lua::lua
        Orgki::libOrgki
        # Orgki::libOrgkiNet
)

if (ORGKI_GUI)
    target_link_libraries(Orgki
        PRIVATE
            Qt6::Gui
            Qt6::Widgets
    )

    qt_generate_deploy_app_script(
        TARGET Orgki
        OUTPUT_SCRIPT deploy_script
    )
    add_custom_command(TARGET Orgki POST_BUILD
        COMMAND ${CMAKE_COMMAND} -P ${deploy_script}
        COMMENT "Running Qt deployment script")
endif()


target_include_directories(Orgki
    PRIVATE
        ${ORGKI_HEADERS_DIR}
)
