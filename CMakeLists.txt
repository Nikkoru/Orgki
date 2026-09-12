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

# find_package(Qt6 COMPONENTS Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(ORGKI_HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")

option(ORGKI_CLI "builds a CLI instead of TUI" OFF)

add_subdirectory(externals)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)

add_executable(Orgki
    ${Orgki_SOURCES}
    ${Orgki_HEADERS}
)

set_property(TARGET Orgki PROPERTY CXX_STANDARD 23)
target_compile_features(Orgki PRIVATE cxx_std_23)

if (ORGKI_CLI)
    target_compile_definitions(Orgki PRIVATE ORGKI_BUILD_CLI)
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
        # Qt6::Gui
        # Qt6::Widgets
)


target_include_directories(Orgki
    PRIVATE
        ${ORGKI_HEADERS_DIR}
)
