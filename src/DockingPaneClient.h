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

#ifndef DOCKINGPANECLIENT_H
#define DOCKINGPANECLIENT_H

#include "DockingPaneBase.h"

class QGridLayout;

class DockingPaneManager;

class DockingPaneClient : public DockingPaneBase
{
    Q_OBJECT

    public:
        DockingPaneClient(QWidget *parent = nullptr);

        friend class DockingPaneManager;

        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false) override;

    private:
        void setWidget(QWidget *widget);
};

#endif // DOCKINGPANECLIENT_H
