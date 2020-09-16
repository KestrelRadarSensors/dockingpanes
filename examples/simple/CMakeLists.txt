set(CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/../lib)

add_executable(simple
    mainwindow.ui
    mainwindow.cpp
    main.cpp
)

set_target_properties(simple
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_link_libraries(simple
    PUBLIC
        dockingpanes
)

install(TARGETS simple)
