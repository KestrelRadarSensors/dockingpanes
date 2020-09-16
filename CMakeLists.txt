cmake_minimum_required(VERSION 3.15)

project(dockingpanes VERSION 1.0.0 DESCRIPTION "Docking windows library for Qt")

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_EXAMPLES "Build examples" ON)

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

if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Declare library target
add_library(dockingpanes)

set_target_properties(dockingpanes
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER src/DockingPaneManager.h
)

set_target_properties(dockingpanes
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_definitions(dockingpanes PUBLIC DOCKINGPANES_LIBRARY)

add_subdirectory(src)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

target_link_libraries(
    dockingpanes
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Xml
)

target_include_directories(
    dockingpanes PUBLIC src/
)

install(TARGETS dockingpanes
    PUBLIC_HEADER DESTINATION include/dockingpanes
)
