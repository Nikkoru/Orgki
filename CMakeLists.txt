cmake_minimum_required(VERSION 3.5)

project("Orgki")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 COMPONENTS Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(ORGKI_HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")

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
        Orgki::libOrgki
        # Orgki::libOrgkiNet
        # Qt6::Gui
        # Qt6::Widgets
)


target_include_directories(Orgki
    PRIVATE
        ${ORGKI_HEADERS_DIR}
)
