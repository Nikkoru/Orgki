set(libOrgki_SOURCES
    "${PROJECT_SOURCE_DIR}/lib/libOrgki/Plan.cpp"
)

add_library(libOrgki
    ${libOrgki_SOURCES}
    ${libOrgki_HEADERS}
)

add_library(Orgki::libOrgki ALIAS libOrgki)

target_include_directories(libOrgki
    PRIVATE
        ${ORGKI_HEADERS_DIR}/libOrgki
)
