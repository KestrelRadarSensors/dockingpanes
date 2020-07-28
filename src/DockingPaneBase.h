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

#ifndef DOCKINGPANEBASE_H
#define DOCKINGPANEBASE_H

#include <QWidget>

class QDomNode;

class DockingPaneManager;

class DockingPaneBase : public QWidget
{
    Q_OBJECT

    public:
        enum State
        {
            Hidden,
            Docked,
            Floating,
            Pinned,
            Tabbed
        };

    public:
        DockingPaneBase(QWidget *parent=NULL);

        friend class DockingPaneManager;

        QString name(void);
        QString id(void);

        DockingPaneManager *dockingManager(void);

        virtual State state(void);
        virtual void setState(DockingPaneBase::State state);
        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false) = 0;

    protected:
        virtual void setName(QString name);
        virtual void setId(QString id);

    protected:
        bool m_isClient;

        State m_state;

        DockingPaneManager *m_dockingManager;

        QString m_name;
        QString m_id;
};

#endif // DOCKINGPANEBASE_H
