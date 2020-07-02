#
# This file is part of DockingPanes. (https://github.com/KestrelRadarSensors/dockingpanes)
#
#  (C) 2020 Kestrel Radar Sensors (https://www.kestrelradarsensors.com)
#
#  DockingPanes is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  DockingPanes is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with DockingPanes.  If not, see <https://www.gnu.org/licenses/>.
#

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simple
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += \
        $$PWD/../../src

CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../../src/debug/ -lDockingPanesd
} else {
    LIBS += -L$$OUT_PWD/../../src/release/ -lDockingPanes
}
