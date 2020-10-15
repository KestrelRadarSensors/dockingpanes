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

#include <QGridLayout>
#include <QStackedWidget>

#include "DockingPaneContainer.h"
#include "DockingPaneTabWidget.h"

DockingPaneTabWidget::DockingPaneTabWidget(QWidget *parent) :
    QWidget(parent), m_stackedWidget(new QStackedWidget())
{
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_stackedWidget);
    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
}

void DockingPaneTabWidget::addPane(DockingPaneContainer *pane)
{
    m_stackedWidget->addWidget(pane->clientWidget());
}
