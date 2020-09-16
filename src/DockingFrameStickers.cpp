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
#include <QPainter>

#include "DockingFrameFrameSticker.h"
#include "DockingFrameStickers.h"
#include "DockingPaneManager.h"

DockingFrameStickers::DockingFrameStickers(QWidget *parent) :
    QWidget(parent),
    m_isActive(false),
    m_tabVisible(false)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setBaseSize(41+24+24, 41+24+24);
    setMaximumSize(41+24+24, 41+24+24);
    setMinimumSize(41+24+24, 41+24+24);

    initializeStickersImages();

    m_rcCentre.setTopLeft(QPoint((this->width()/2)-(m_activeStickers.value(Centre).width()/2),(this->height()/2)-(m_activeStickers.value(Centre).height()/2)));
    m_rcCentre.setSize(m_activeStickers.value(Centre).size());

    m_rcLeft.setTopLeft(QPoint(0,(this->height()/2)-(m_activeStickers.value(Left).height()/2)));
    m_rcLeft.setSize(m_activeStickers.value(Left).size());

    m_rcRight.setTopLeft(QPoint(this->width()-m_activeStickers.value(Right).width(),(this->height()/2)-(m_activeStickers.value(Right).height()/2)));
    m_rcRight.setSize(m_activeStickers.value(Right).size());

    m_rcTop.setTopLeft(QPoint((this->width()/2)-(m_activeStickers.value(Top).width()/2), 0));
    m_rcTop.setSize(m_activeStickers.value(Top).size());

    m_rcBottom.setTopLeft(QPoint((this->width()/2)-(m_activeStickers.value(Top).width()/2), (this->height()-m_activeStickers.value(Top).height())));
    m_rcBottom.setSize(m_activeStickers.value(Bottom).size());

    m_rcTab.setTopLeft(QPoint((this->width()/2)-(m_activeStickers.value(Tab).width()/2),(this->height()/2)-(m_activeStickers.value(Tab).height()/2)));
    m_rcTab.setSize(m_activeStickers.value(Tab).size());

    m_frameLeftSticker = new DockingFrameFrameSticker("frame_left", this);
    m_frameRightSticker = new DockingFrameFrameSticker("frame_right", this);
    m_frameTopSticker = new DockingFrameFrameSticker("frame_top", this);
    m_frameBottomSticker = new DockingFrameFrameSticker("frame_bottom", this);
}

void DockingFrameStickers::paintEvent(QPaintEvent*)
{
     QPainter p(this);

     p.drawImage(m_rcCentre, m_isActive ? m_activeStickers.value(Centre) : m_inactiveStickers.value(Centre));
     p.drawImage(m_rcLeft, m_isActive ? m_activeStickers.value(Left) : m_inactiveStickers.value(Left));
     p.drawImage(m_rcRight, m_isActive ? m_activeStickers.value(Right) : m_inactiveStickers.value(Right));
     p.drawImage(m_rcTop, m_isActive ? m_activeStickers.value(Top) : m_inactiveStickers.value(Top));
     p.drawImage(m_rcBottom, m_isActive ? m_activeStickers.value(Bottom) : m_inactiveStickers.value(Bottom));

     if (m_tabVisible) {
         p.drawImage(m_rcTab, m_isActive ? m_activeStickers.value(Tab) : m_inactiveStickers.value(Tab));
     }
}

void DockingFrameStickers::setTabVisible(bool state)
{
    if (m_tabVisible!=state) {
        m_tabVisible = state;

        update();
    }
}

bool DockingFrameStickers::getHit(QPoint pos, DockingFrameStickers::DockingPosition *dockPos)
{
    if (m_tabVisible && m_rcTab.contains(this->mapFromGlobal(pos))) {
        *dockPos = tab;

        return(true);
    }

    if (m_rcLeft.contains(this->mapFromGlobal(pos))) {
        *dockPos = paneLeft;

        return(true);
    }

    if (m_rcRight.contains(this->mapFromGlobal(pos))) {
        *dockPos = paneRight;

        return(true);
    }

    if (m_rcTop.contains(this->mapFromGlobal(pos))) {
        *dockPos = paneTop;

        return(true);
    }

    if (m_rcBottom.contains(this->mapFromGlobal(pos))) {
        *dockPos = paneBottom;

        return(true);
    }

    if (m_frameLeftSticker->rect().contains(m_frameLeftSticker->mapFromGlobal(pos))) {
        *dockPos = frameLeft;

        return(true);
    }

    if (m_frameRightSticker->rect().contains(m_frameRightSticker->mapFromGlobal(pos))) {
        *dockPos = frameRight;

        return(true);
    }

    if (m_frameTopSticker->rect().contains(m_frameTopSticker->mapFromGlobal(pos))) {
        *dockPos = frameTop;

        return(true);
    }

    if (m_frameBottomSticker->rect().contains(m_frameBottomSticker->mapFromGlobal(pos))) {
        *dockPos = frameBottom;

        return(true);
    }

    return(false);
}

void DockingFrameStickers::hideEvent(QHideEvent*)
{
    m_frameLeftSticker->hide();
    m_frameRightSticker->hide();
    m_frameTopSticker->hide();
    m_frameBottomSticker->hide();
}

void DockingFrameStickers::showEvent(QShowEvent*)
{
    m_frameLeftSticker->show();
    m_frameRightSticker->show();
    m_frameTopSticker->show();
    m_frameBottomSticker->show();
}

void DockingFrameStickers::initializeStickersImages()
{
    m_activeStickers.insert(Centre, QImage(":/img/docking_bitmaps/centre_active.png"));
    m_activeStickers.insert(Left, QImage(":/img/docking_bitmaps/window_left_active.png"));
    m_activeStickers.insert(Right, QImage(":/img/docking_bitmaps/window_right_active.png"));
    m_activeStickers.insert(Top, QImage(":/img/docking_bitmaps/window_top_active.png"));
    m_activeStickers.insert(Bottom, QImage(":/img/docking_bitmaps/window_bottom_active.png"));
    m_activeStickers.insert(Tab, QImage(":/img/docking_bitmaps/tab.png"));

    m_inactiveStickers.insert(Centre, QImage(":/img/docking_bitmaps/centre_inactive.png"));
    m_inactiveStickers.insert(Left, QImage(":/img/docking_bitmaps/window_left_inactive.png"));
    m_inactiveStickers.insert(Right, QImage(":/img/docking_bitmaps/window_right_inactive.png"));
    m_inactiveStickers.insert(Top, QImage(":/img/docking_bitmaps/window_top_inactive.png"));
    m_inactiveStickers.insert(Bottom, QImage(":/img/docking_bitmaps/window_bottom_inactive.png"));
    m_inactiveStickers.insert(Tab, QImage(":/img/docking_bitmaps/tab.png"));
}

void DockingFrameStickers::setFrameRect(QRect rect)
{
    m_frameLeftSticker->move(QPoint(rect.left(), rect.center().y()-(m_frameLeftSticker->height()/2)));
    m_frameTopSticker->move(QPoint(rect.center().x()-(m_frameTopSticker->width()/2), rect.top()));
    m_frameBottomSticker->move(QPoint(rect.center().x()-(m_frameTopSticker->width()/2), rect.bottom()-m_frameBottomSticker->height()));
    m_frameRightSticker->move(QPoint(rect.right()-m_frameRightSticker->width(), rect.center().y()-(m_frameLeftSticker->height()/2)));
}

void DockingFrameStickers::updateCursorPos(QPoint pos)
{
    m_frameLeftSticker->updateCursorPos(pos);
    m_frameRightSticker->updateCursorPos(pos);
    m_frameTopSticker->updateCursorPos(pos);
    m_frameBottomSticker->updateCursorPos(pos);

    if  ( (m_rcLeft.contains(this->mapFromGlobal(pos))) ||
          (m_rcRight.contains(this->mapFromGlobal(pos))) ||
          (m_rcTop.contains(this->mapFromGlobal(pos))) ||
          (m_rcBottom.contains(this->mapFromGlobal(pos))) ||
          (m_tabVisible && m_rcTab.contains(this->mapFromGlobal(pos))) ) {
        if (!m_isActive) {
            m_isActive = true;

            update();
        }
    } else {
        if (m_isActive) {
            m_isActive = false;

            update();
        }
    }
}

