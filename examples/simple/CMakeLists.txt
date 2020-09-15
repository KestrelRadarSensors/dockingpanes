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

target_link_libraries(
    simple
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        dockingpanes
)

install(TARGETS simple
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)