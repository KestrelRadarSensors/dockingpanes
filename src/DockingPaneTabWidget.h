/*
 * This file is part of DockingPanes. (https://github.com/KestrelRadarSensors/dockingpanes)
 *
 * (C) 2020 Kestrel Radar Sensors (https://www.kestrelradarsensors.com)
 *
 * DockingPanes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DockingPanes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with DockingPanes.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DOCKINGPANETABWIDGET_H
#define DOCKINGPANETABWIDGET_H

#include <QWidget>

class QStackedWidget;

class DockingPaneContainer;

class DockingPaneTabWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit DockingPaneTabWidget(QWidget *parent = nullptr);
        void addPane(DockingPaneContainer *pane);

    private:
        QStackedWidget *m_stackedWidget;
};

#endif // DOCKINGPANETABWIDGET_H
