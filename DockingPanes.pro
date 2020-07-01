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

TEMPLATE = lib

CONFIG -= create_prl

CONFIG(debug, debug|release) {
    mac: TARGET = $$join(TARGET,,,_debug)
    win32: TARGET = $$join(TARGET,,,d)
}

QT += widgets core gui xml

DEFINES += DOCKINGPANES_LIBRARY

SOURCES += \
    DockingPaneManager.cpp \
    DockingPaneBase.cpp \
    DockingPaneClient.cpp \
    DockingPaneContainer.cpp \
    DockingPaneSplitterContainer.cpp \
    DockAutoHidebutton.cpp \
    DockingPaneTitleWidget.cpp \
    DockingToolButton.cpp \
    DockingFrameFrameSticker.cpp \
    DockingFrameStickers.cpp \
    DockingPaneTabbedContainer.cpp \
    DockingPaneTabWidget.cpp \
    DockingPaneFlyoutWidget.cpp \
    DockingTargetWidget.cpp \
    DockingPaneGlowWidget.cpp \
    DockingPaneGlow.cpp

HEADERS += \
    DockingPaneManager.h\
    DockingPanes_global.h \
    DockingPaneBase.h \
    DockingPaneClient.h \
    DockingPaneContainer.h \
    DockingPaneSplitterContainer.h \
    DockAutoHideButton.h \
    DockingPaneTitleWidget.h \
    DockingToolButton.h \
    DockingFrameFrameSticker.h \
    DockingFrameStickers.h \
    DockingPaneTabbedContainer.h \
    DockingPaneTabWidget.h \
    DockingPaneFlyoutWidget.h \
    DockingTargetWidget.h \
    DockingPaneGlowWidget.h \
    DockingPaneGlow.h

RESOURCES += \
    images.qrc

