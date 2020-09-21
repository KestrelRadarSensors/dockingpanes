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
#include <QGridLayout>

#include "DockingPaneClient.h"

DockingPaneClient::DockingPaneClient(QWidget *parent) :
    DockingPaneBase(parent)
{
    this->setLayout(new QGridLayout());
}

void DockingPaneClient::setWidget(QWidget *widget)
{
    delete this->layout();
    this->setLayout(new QGridLayout());
    this->layout()->setMargin(0);
    this->layout()->setSpacing(0);
    this->layout()->addWidget(widget);
}

void DockingPaneClient::saveLayout(QDomNode *parentNode, bool)
{
    QDomDocument doc = parentNode->ownerDocument();
    QDomElement domElement = doc.createElement(this->metaObject()->className());
    parentNode->appendChild(domElement);
}
