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
    QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_PaintOnScreen);

    setBaseSize(41+24+24, 41+24+24);
    setMaximumSize(41+24+24, 41+24+24);
    setMinimumSize(41+24+24, 41+24+24);

    m_isActive = false;

    QImage centre;
    QImage left;
    QImage top;
    QImage right;
    QImage bottom;
    QImage tab;

    centre = QImage(":/dockingBitmaps/centre_active.png");
    left  = QImage(":/dockingBitmaps/window_left_active.png");
    top  = QImage(":/dockingBitmaps/window_top_active.png");
    right  = QImage(":/dockingBitmaps/window_right_active.png");
    bottom  = QImage(":/dockingBitmaps/window_bottom_active.png");
    tab  = QImage(":/dockingBitmaps/tab.png");

    m_rcCentre.setTopLeft(QPoint((this->width()/2)-(centre.width()/2),(this->height()/2)-(centre.height()/2)));
    m_rcCentre.setSize(centre.size());

    m_rcLeft.setTopLeft(QPoint(0,(this->height()/2)-(left.height()/2)));
    m_rcLeft.setSize(left.size());

    m_rcRight.setTopLeft(QPoint(this->width()-right.width(),(this->height()/2)-(left.height()/2)));
    m_rcRight.setSize(right.size());

    m_rcTop.setTopLeft(QPoint((this->width()/2)-(top.width()/2), 0));
    m_rcTop.setSize(top.size());

    m_rcBottom.setTopLeft(QPoint((this->width()/2)-(top.width()/2), (this->height()-top.height())));
    m_rcBottom.setSize(bottom.size());

    m_rcTab.setTopLeft(QPoint((this->width()/2)-(tab.width()/2),(this->height()/2)-(tab.height()/2)));
    m_rcTab.setSize(tab.size());

    m_frameLeftSticker = new DockingFrameFrameSticker("frame_left");
    m_frameRightSticker = new DockingFrameFrameSticker("frame_right");
    m_frameTopSticker = new DockingFrameFrameSticker("frame_top");
    m_frameBottomSticker = new DockingFrameFrameSticker("frame_bottom");
}

void DockingFrameStickers::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    QImage centre;
    QImage left;
    QImage top;
    QImage right;
    QImage bottom;
    QImage tab;

    if (m_isActive) {
        centre = QImage(":/dockingBitmaps/centre_active.png");
        left  = QImage(":/dockingBitmaps/window_left_active.png");
        top  = QImage(":/dockingBitmaps/window_top_active.png");
        right  = QImage(":/dockingBitmaps/window_right_active.png");
        bottom  = QImage(":/dockingBitmaps/window_bottom_active.png");
        tab  = QImage(":/dockingBitmaps/tab.png");
    } else {
        centre = QImage(":/dockingBitmaps/centre_inactive.png");
        left  = QImage(":/dockingBitmaps/window_left_inactive.png");
        top  = QImage(":/dockingBitmaps/window_top_inactive.png");
        right  = QImage(":/dockingBitmaps/window_right_inactive.png");
        bottom  = QImage(":/dockingBitmaps/window_bottom_inactive.png");
        tab  = QImage(":/dockingBitmaps/tab.png");
    }

    p.drawImage(m_rcCentre, centre);
    p.drawImage(m_rcLeft, left);
    p.drawImage(m_rcRight, right);
    p.drawImage(m_rcTop, top);
    p.drawImage(m_rcBottom, bottom);

    if (m_tabVisible) {
        p.drawImage(m_rcTab, tab);
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

