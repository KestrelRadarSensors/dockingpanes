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

#ifndef DOCKINGPANESPLITTERCONTAINER_H
#define DOCKINGPANESPLITTERCONTAINER_H

#include "DockingPaneBase.h"

class QSplitter;

class DockingPaneManager;

class DockingPaneSplitterContainer : public DockingPaneBase
{
    Q_OBJECT

    friend class DockingPaneManager;

    public:
        enum SplitterDirection
        {
            splitVertical,
            splitHorizontal
        };

        DockingPaneSplitterContainer(QWidget* parent = nullptr, SplitterDirection direction=splitVertical);
        virtual ~DockingPaneSplitterContainer() = default;

        SplitterDirection direction(void);

        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false) override;

    private:
        QSplitter *m_splitterWidget;
};

#endif // DOCKINGPANESPLITTERCONTAINER_H
