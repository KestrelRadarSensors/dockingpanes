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

#include "DockingPaneBase.h"
#include <QDebug>

DockingPaneBase::DockingPaneBase(QWidget *parent) :
    QWidget(parent), m_state(Hidden)
{
}

QString DockingPaneBase::name(void)
{
    return(m_name);
}

QString DockingPaneBase::id(void)
{
    return(m_id);
}

void DockingPaneBase::setName(QString name)
{
    m_name = name;
}

void DockingPaneBase::setId(QString id)
{
    m_id = id;
}

DockingPaneManager *DockingPaneBase::dockingManager(void)
{
    return(m_dockingManager);
}

DockingPaneBase::State DockingPaneBase::state(void)
{
    return(m_state);
}

void DockingPaneBase::setState(DockingPaneBase::State state)
{
    m_state = state;
}
