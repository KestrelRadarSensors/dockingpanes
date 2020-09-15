cmake_minimum_required(VERSION 3.15)

project(dockingpanes)

# Required dependencies
find_package(
    Qt5
    COMPONENTS
        Core
        Gui
        Widgets
        Xml
    REQUIRED
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(dockingpanes STATIC)

set_target_properties(dockingpanes
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_sources(
    dockingpanes
    PUBLIC
        DockAutoHideButton.h
        DockingFrameFrameSticker.h
        DockingFrameStickers.h
        DockingPaneBase.h
        DockingPaneClient.h
        DockingPaneContainer.h
        DockingPaneFlyoutWidget.h
        DockingPaneGlow.h
        DockingPaneGlowWidget.h
        DockingPaneManager.h
        DockingPanes_global.h
        DockingPaneSplitterContainer.h
        DockingPaneTabbedContainer.h
        DockingPaneTabWidget.h
        DockingPaneTitleWidget.h
        DockingTargetWidget.h
        DockingToolButton.h
    PRIVATE
        DockAutoHideButton.cpp
        DockingFrameFrameSticker.cpp
        DockingFrameStickers.cpp
        DockingPaneBase.cpp
        DockingPaneClient.cpp
        DockingPaneContainer.cpp
        DockingPaneFlyoutWidget.cpp
        DockingPaneGlow.cpp
        DockingPaneGlowWidget.cpp
        DockingPaneManager.cpp
        DockingPaneSplitterContainer.cpp
        DockingPaneTabbedContainer.cpp
        DockingPaneTabWidget.cpp
        DockingPaneTitleWidget.cpp
        DockingTargetWidget.cpp
        DockingToolButton.cpp
        # Resouces (needs autorcc)
        resources/images.qrc
)

target_link_libraries(
    dockingpanes
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Xml
)

# install(TARGETS dockingpanes DESTINATION lib)
# install(FILES ???)

# cmake_minimum_required(VERSION 3.13)
# include(GNUInstallDirs)
# project(libfoo
#     VERSION 0.1.0
#     LANGUAGES CXX
# )
# add_library(foo SHARED
#     source/foo.cpp
# )
# target_include_directories(foo PUBLIC
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/foo>
# )
# target_compile_features(foo PRIVATE cxx_std_17)
# target_compile_options (foo PRIVATE -Wall -Wextra)
# set_target_properties  (foo
#     PROPERTIES
#         PUBLIC_HEADER "include/foo.h")
# install(TARGETS foo EXPORT fooTargets
#     LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foo
# )
# install(EXPORT fooTargets
#     FILE fooTargets.cmake
#     NAMESPACE foo::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foo
# )