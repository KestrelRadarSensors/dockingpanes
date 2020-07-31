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

#include <QDebug>

#include "DockingPaneGlow.h"
#include "DockingPaneGlowWidget.h"

DockingPaneGlow::DockingPaneGlow(QWidget* floatingPane, QObject* parent) :
    QObject(parent)
{
    m_leftGlow = new DockingPaneGlowWidget(floatingPane, DockingPaneGlowWidget::Left, qobject_cast<QWidget*>(parent));
    connect(m_leftGlow, &DockingPaneGlowWidget::glowResized, this, &DockingPaneGlow::onGlowResized);
    m_leftGlow->show();

    m_rightGlow = new DockingPaneGlowWidget(floatingPane, DockingPaneGlowWidget::Right,  qobject_cast<QWidget*>(parent));
    connect(m_rightGlow, &DockingPaneGlowWidget::glowResized, this, &DockingPaneGlow::onGlowResized);
    m_rightGlow->show();

    m_topGlow = new DockingPaneGlowWidget(floatingPane, DockingPaneGlowWidget::Top,  qobject_cast<QWidget*>(parent));
    connect(m_topGlow, &DockingPaneGlowWidget::glowResized, this, &DockingPaneGlow::onGlowResized);
    m_topGlow->show();

    m_bottomGlow = new DockingPaneGlowWidget(floatingPane, DockingPaneGlowWidget::Bottom,  qobject_cast<QWidget*>(parent));
    connect(m_bottomGlow, &DockingPaneGlowWidget::glowResized, this, &DockingPaneGlow::onGlowResized);
    m_bottomGlow->show();
}

DockingPaneGlow::~DockingPaneGlow()
{
    delete m_leftGlow;
    delete m_rightGlow;
    delete m_topGlow;
    delete m_bottomGlow;
}

void DockingPaneGlow::onGlowResized(void)
{
    m_leftGlow->updatePosition();
    m_rightGlow->updatePosition();
    m_topGlow->updatePosition();
    m_bottomGlow->updatePosition();
}

void DockingPaneGlow::raise(void)
{
    this->m_leftGlow->raise();
    this->m_rightGlow->raise();
    this->m_topGlow->raise();
    this->m_bottomGlow->raise();
}

void DockingPaneGlow::update(void)
{
    m_leftGlow->updatePosition();
    m_rightGlow->updatePosition();
    m_topGlow->updatePosition();
    m_bottomGlow->updatePosition();
}
