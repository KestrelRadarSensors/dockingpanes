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

#include <QDomDocument>
#include <QDebug>
#include <QGridLayout>
#include <QSplitter>

#include "DockingPaneSplitterContainer.h"

DockingPaneSplitterContainer::DockingPaneSplitterContainer(QWidget *parent, SplitterDirection direction) :
    DockingPaneBase(parent)
{
    QGridLayout *layout = new QGridLayout();

    m_splitterWidget = new QSplitter();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_splitterWidget);

    if (direction==splitVertical) {
        m_splitterWidget->setOrientation(Qt::Vertical);
    } else {
        m_splitterWidget->setOrientation(Qt::Horizontal);
    }

    m_splitterWidget->setChildrenCollapsible(false);

    this->setLayout(layout);
}

DockingPaneSplitterContainer::SplitterDirection DockingPaneSplitterContainer::direction(void)
{
    if (m_splitterWidget->orientation()==Qt::Vertical) {
        return(splitVertical);
    } else {
        return(splitHorizontal);
    }
}

void DockingPaneSplitterContainer::saveLayout(QDomNode *parentNode, bool)
{
    QDomDocument doc = parentNode->ownerDocument();

    QDomElement domElement = doc.createElement(this->metaObject()->className());

    domElement.setAttribute("state", (QString)m_splitterWidget->saveState().toBase64());

    for(int i=0;i<m_splitterWidget->count();i++) {
        DockingPaneBase *childPane = qobject_cast<DockingPaneBase *>(m_splitterWidget->widget(i));

        if (childPane) {
            childPane->saveLayout(&domElement);
        }
    }

    parentNode->appendChild(domElement);
}
