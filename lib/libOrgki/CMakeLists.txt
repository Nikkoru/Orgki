set(libOrgki_SOURCES
    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Plan.cpp"
    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Table.cpp"

    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Time/Time.cpp"

    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Commands/Parser.cpp"
)

add_library(libOrgki
    ${libOrgki_SOURCES}
    ${libOrgki_HEADERS}
)

add_library(Orgki::libOrgki ALIAS libOrgki)
target_link_libraries(libOrgki
    PRIVATE
        nlohmann_json
)

target_include_directories(libOrgki
    PRIVATE
        ${ORGKI_HEADERS_DIR}/libOrgki
)
