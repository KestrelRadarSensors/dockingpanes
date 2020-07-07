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
#include <QPaintEvent>
#include <QPainter>

#include "DockingFrameFrameSticker.h"

DockingFrameFrameSticker::DockingFrameFrameSticker(QString image, QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );

    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_PaintOnScreen);

    m_activeImage = QImage(QString(":/dockingBitmaps/%1_active.png").arg(image));
    m_inactiveImage = QImage(QString(":/dockingBitmaps/%1_inactive.png").arg(image));

    this->setBaseSize(m_activeImage.size());
    this->setMaximumSize(m_activeImage.size());
    this->setMaximumSize(m_activeImage.size());

    m_isActive = false;
}

void DockingFrameFrameSticker::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);

    if (m_isActive) {
        p.drawImage(0, 0, m_activeImage);
    } else {
        p.drawImage(0, 0, m_inactiveImage);
    }
}

void DockingFrameFrameSticker::updateCursorPos(QPoint pos)
{
    bool oldActive = m_isActive;

    if (this->rect().contains(mapFromGlobal(pos))) {
        m_isActive = true;
    } else {
        m_isActive = false;
    }

    if (oldActive!=m_isActive) {
        update();
    }
}
