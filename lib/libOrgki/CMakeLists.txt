set(libOrgki_SOURCES
    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Plan.cpp"
    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Table.cpp"

    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Settings/SettingsData.cpp"
    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Settings/SettingsManager.cpp"

    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Time/Time.cpp"

    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Commands/Parser.cpp"
)

add_library(libOrgki
    ${libOrgki_SOURCES}
    ${libOrgki_HEADERS}
)

add_library(Orgki::libOrgki ALIAS libOrgki)
set_target_properties(libOrgki PROPERTIES PREFIX "")

target_link_libraries(libOrgki
    PUBLIC 
        nlohmann_json::nlohmann_json
)

target_include_directories(libOrgki
    PRIVATE
        "${ORGKI_HEADERS_DIR}/libOrgki"
)
