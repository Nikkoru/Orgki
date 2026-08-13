cmake_minimum_required (VERSION 3.5)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Orgki")

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(lib)

add_executable (Orgki
    ${SOURCES}
    ${HEADERS}
)

set_property(TARGET Orgki PROPERTY CXX_STANDARD 23)
target_compile_features(Orgki PRIVATE cxx_std_23)

if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
endif()
