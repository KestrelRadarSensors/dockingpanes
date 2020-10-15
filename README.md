# DockingPanes

DockingPanes is a library for Qt Widgets that implements docking windows that have the look and feel of Visual Studio.  It provides a simple API which allows an application to make use of docking windows with a few calls.

![Docking panes example](https://user-images.githubusercontent.com/67605614/86236792-8f2e0a80-bb92-11ea-982c-c66ea48aa4fe.png)

## Features

* Provides a central widget area for your main application windows.
* Allows docking windows to be docked in any position around the central widget.
* Allows docking windows to be docked in a tabbed "pane" to maximise space available for the central widget.
* Allows docking window panes to be collapsed into a button to the top, left, right or bottom of the central widget depending on the position of the docking window in the hierarchy.  A fly-out mechanism allows the windows to be access by hovering over the button.
* Positions of windows can be stored to a variable or file which allows different configurations of windows per application context to be used and saved and restored across application runs.

## Compatability

The library has only been tested under windows and may require tweaking of drawing for other platforms, however, there is no platform specific code.

## Compiling

### qmake
A qmake project file is provided to build the library, this allows the library to be build either inside QtCreator by opening the project or by using qmake from the command line and building native files for your build system.

Debug builds of the library are suffixed with a per platform string to allow debug and release builds to exist side by side.

### cmake

A `CMakeLists.txt` file is provided on the root folder. The current options supported are:

* `-DBUILD_SHARED_LIBS`: If `ON`, then the result will be a shared lib (`*.so`/`*.dll` file). If `OFF`, the result will be a static lib (`*.a`/`*.lib` file). By default is `ON`.
* `-DBUILD_EXAMPLES`: If `ON`, It will build the applications inside `examples` folder. By default is `ON`.

#### Example:
```
cd _build/
cmake -DCMAKE_INSTALL_PREFIX=/your/desired/location -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j<number_of_cores>
cmake --install .
```

Then, the directory `/your/desired/location` will be (on linux):
```
.
├── bin
│   └── simple
├── include
│   └── dockingpanes
│       └── DockingPaneManager.h
└── lib
    ├── libdockingpanes.so -> libdockingpanes.so.1
    ├── libdockingpanes.so.1 -> libdockingpanes.so.1.0.0
    └── libdockingpanes.so.1.0.0

```

## Documentation

The API is designed to be simple to use and as such there is currently no documentation available.  The DockingManager class contains the most useful calls.

We will add doxygen documentation as and when we are able to, however this is not a high priority.

## Usage

The following is a short basic example of initialising the docking manager, creating a tool window and setting the central widget of the MainWindow to the docking manager widget.

```c++
auto centralWidget = new QLabel("Hello World");
auto dockingManager = new DockingPaneManager;

dockingManager->setClientWidget(centralWidget);

auto toolWindowWidget = new QLabel("Hola World");
auto initialSize = QSize(200,100);

auto dockingWindow = dockingManager->createPane("myPaneId", "Window Title", toolWindowWidget, initialSize, DockingManager::dockFloat, nullptr);

// the window is now created and managed by the docking manager.  The ToolWindow returned can be used with other API calls to open or close the window and other functions.

setCentralWidget(dockingManager->widget());

```

## License

DockingPanes is licensed under the GPLv3 License, if you would like to use the library and this is not compatible then you should contact us to discuss options.
