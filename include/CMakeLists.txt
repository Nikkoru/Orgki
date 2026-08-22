add_subdirectory(Orgki)
add_subdirectory(libOrgki)
add_subdirectory(libOrgkiNet)

set(Orgki_HEADERS
    ${Orgki_HEADERS}
    PARENT_SCOPE)

set(libOrgki_HEADERS
    ${libOrgki_HEADERS}
    PARENT_SCOPE)

set(libOrgkiNet_HEADERS
    ${libOrgkiNet_HEADERS}
    PARENT_SCOPE)
