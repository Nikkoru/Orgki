cmake_minimum_required(VERSION 3.5)

project("Orgki")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(ORGKI_HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(externals)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(lib)

find_package(Qt6 COMPONENTS Gui Widgets REQUIRED)

add_executable(Orgki
    ${SOURCES}
    ${HEADERS}
)

set_property(TARGET Orgki PROPERTY CXX_STANDARD 23)
target_compile_features(Orgki PRIVATE cxx_std_23)

if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
endif()

target_link_libraries(Orgki
    PRIVATE
    stdc++exp
    Orgki::libOrgki
    # Orgki::libOrgkiNet
    Qt6::Gui
    Qt6::Widgets
)

target_include_directories(Orgki
    PRIVATE
        ${ORGKI_HEADERS_DIR}
)
