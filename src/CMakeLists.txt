set(Orgki_SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
PARENT_SCOPE)

if (ORGKI_GUI)
    list(APPEND
       Orgki_SOURCES 
            "${PROJECT_SOURCE_DIR}/src/main.cpp"
            "${PROJECT_SOURCE_DIR}/src/MainWindow.cpp"
            "${PROJECT_SOURCE_DIR}/src/Views/MainView.cpp"
            "${PROJECT_SOURCE_DIR}/src/Views/TabView.cpp"
            "${PROJECT_SOURCE_DIR}/src/Views/TreeView.cpp"
    )
endif()

set(Orgki_SOURCES ${Orgki_SOURCES} PARENT_SCOPE)
