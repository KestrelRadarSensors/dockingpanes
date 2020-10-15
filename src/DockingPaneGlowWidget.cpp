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

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>

#include "DockingPaneGlowWidget.h"

DockingPaneGlowWidget::DockingPaneGlowWidget(QWidget *floatingPane, DockingPaneGlowWidget::Position pos, QWidget *parent) :
    QWidget(parent),
    m_floatingPane(floatingPane),
    m_position(pos),
    m_dragging(false)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocusProxy(floatingPane);
    this->setWindowOpacity(0.01);
    this->setMouseTracking(true);

    updatePosition();
}

void DockingPaneGlowWidget::updatePosition(void)
{
    QSize size = m_floatingPane->size();
    QPoint pos = m_floatingPane->pos();

    switch(m_position) {
        case DockingPaneGlowWidget::Left: {
            this->move(pos.x()-9, pos.y());
            this->resize(9, size.height());
            break;
        }
        case DockingPaneGlowWidget::Right: {
            this->move(pos.x()+size.width(), pos.y());
            this->resize(9, size.height());
            break;
        }
        case DockingPaneGlowWidget::Top: {
            this->move(pos.x()-9, pos.y()-9);
            this->resize(size.width()+18, 9);
            break;
        }
        case DockingPaneGlowWidget::Bottom: {
            this->move(pos.x()-9, pos.y()+size.height());
            this->resize(size.width()+18, 9);
            break;
        }
    }
}

void DockingPaneGlowWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_cursorDelta = mapToGlobal(event->pos()).x()-this->pos().x();
        m_cornerState = 0;

        if ( (m_position==DockingPaneGlowWidget::Top) || (m_position==DockingPaneGlowWidget::Bottom) ) {
            if (m_cursorDelta<9) {
                m_cornerState = 1;
            }
            if (m_cursorDelta>this->width()-9) {
                m_cornerState = 2;
            }
        }

        m_Pos = QCursor::pos();
        m_paneGeometry = m_floatingPane->geometry();
        m_dragging = true;
        qApp->setActiveWindow(m_floatingPane);
        grabMouse();
    }
}

void DockingPaneGlowWidget::updateCursor()
{
    int cursorPos = QCursor::pos().x()-this->pos().x();

    switch(m_position) {
        case DockingPaneGlowWidget::Left: {
            setCursor(Qt::SizeHorCursor);
            break;
        }
        case DockingPaneGlowWidget::Right: {
            setCursor(Qt::SizeHorCursor);
            break;
        }
        case DockingPaneGlowWidget::Top: {
            if (cursorPos<9) {
                setCursor(Qt::SizeFDiagCursor);
            } else {
                if (cursorPos>this->width()-9) {
                    setCursor(Qt::SizeBDiagCursor);
                } else {
                    setCursor(Qt::SizeVerCursor);
                }
            }
            break;
        }
        case DockingPaneGlowWidget::Bottom: {
            if (cursorPos<9) {
                setCursor(Qt::SizeBDiagCursor);
            } else {
                if (cursorPos>this->width()-9) {
                    setCursor(Qt::SizeFDiagCursor);
                } else {
                    setCursor(Qt::SizeVerCursor);
                }
            }
            break;
        }
    }
}

void DockingPaneGlowWidget::enterEvent(QEvent*)
{
    updateCursor();
}

void DockingPaneGlowWidget::leaveEvent(QEvent*)
{
    this->unsetCursor();
}

void DockingPaneGlowWidget::mouseMoveEvent(QMouseEvent*)
{
    if (m_dragging) {
        QPoint delta;
        QPoint pos = QCursor::pos();
        delta = m_Pos-pos;
        QRect rc = m_floatingPane->geometry();

        switch(m_position) {
            case DockingPaneGlowWidget::Left: {
                rc.adjust(-delta.x(), 0, 0, 0);
                if (rc.left()<=m_paneGeometry.right()-m_floatingPane->minimumWidth()) {
                    m_floatingPane->setGeometry(rc);
                }
                break;
            }
            case DockingPaneGlowWidget::Right: {
                rc.adjust(0,0,-delta.x(), 0);
                m_floatingPane->setGeometry(rc);
                break;
            }
            case DockingPaneGlowWidget::Top: {
                rc.adjust(0, -delta.y(), 0, 0);
                if (m_cornerState==1) {
                    QRect tempRc = rc;
                    tempRc.adjust(-delta.x(), 0, 0, 0);
                    if (tempRc.left()<=m_paneGeometry.right()-m_floatingPane->minimumWidth()) {
                        rc = tempRc;
                    }
                }
                if (m_cornerState==2) {
                    rc.adjust(0,0,-delta.x(), 0);
                }
                if (rc.top()<=m_paneGeometry.bottom()-m_floatingPane->minimumHeight()) {
                    m_floatingPane->setGeometry(rc);
                }
                break;
            }
            case DockingPaneGlowWidget::Bottom: {
                rc.adjust(0,0,0, -delta.y());
                if (m_cornerState==1) {
                    QRect tempRc = rc;
                    tempRc.adjust(-delta.x(), 0, 0, 0);
                    if (tempRc.left()<=m_paneGeometry.right()-m_floatingPane->minimumWidth()) {
                        rc = tempRc;
                    }
                }
                if (m_cornerState==2) {
                    rc.adjust(0,0,-delta.x(), 0);
                }
                m_floatingPane->setGeometry(rc);
                break;
            }
        }

        updatePosition();
        Q_EMIT glowResized();
        m_Pos = pos;
    } else {
        updateCursor();
    }
}

void DockingPaneGlowWidget::mouseReleaseEvent(QMouseEvent*)
{
    m_dragging = false;
    releaseMouse();
}

void DockingPaneGlowWidget::paintEvent(QPaintEvent*)
{
    return;
}
