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
#include <QFont>
#include <QMouseEvent>
#include <QPainter>

#include "DockingPaneTitleWidget.h"

DockingPaneTitleWidget::DockingPaneTitleWidget(QString text, QWidget *parent) :
    QWidget(parent), m_text(text), m_active(false)
{
    this->setFont(QFont("Segoe UI", 9));
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(qApp, &QApplication::focusChanged, this, &DockingPaneTitleWidget::onFocusChanged);
}

void DockingPaneTitleWidget::resizeEvent(QResizeEvent*)
{
    this->setMinimumHeight(6+this->fontMetrics().height());
    this->setMaximumHeight(6+this->fontMetrics().height());
}

void DockingPaneTitleWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QRect drawnRect;

    int leftMargin, rightMargin;

    leftMargin = 5;
    rightMargin = 5;

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::HighQualityAntialiasing, true);

    QFontMetrics fm(this->font());

    QString elidedText = fm.elidedText(m_text, Qt::ElideRight, width()-6);

    if (m_active) {
        p.setPen(QColor(0xFF, 0xFF, 0xFF));
    } else {
        p.setPen(QColor(0, 0, 0));
    }

    p.drawText(leftMargin, 0, width()-(leftMargin+rightMargin), height(), Qt::AlignVCenter , elidedText, &drawnRect);

    if ((width()-drawnRect.width()-(leftMargin+rightMargin*2))>0) {
        drawPattern(&p, drawnRect.width()+(leftMargin*2), 0, width()-drawnRect.width()-((leftMargin*2)+rightMargin), height());
    }
}

void DockingPaneTitleWidget::setText(QString text)
{
    m_text = text;
}

void DockingPaneTitleWidget::drawPattern(QPainter *p, int x, int y, int w, int h)
{
    QPixmap pixMap(4,5);

    pixMap.fill(Qt::transparent);

    QPainter pp(&pixMap);

    if (m_active) {
        pp.setPen(QColor(0x59, 0xa8, 0xde));
    } else {
        pp.setPen(QColor(0x99, 0x99, 0x99));
    }

    pp.drawPoint(0, 0);
    pp.drawPoint(0, 4);
    pp.drawPoint(2, 2);

    p->setBrushOrigin(x, ((y+h)/2)-2);
    p->fillRect(x,((y+h)/2)-2,w, 5, QBrush(pixMap));
}

void DockingPaneTitleWidget::mouseMoveEvent(QMouseEvent* event)
{
    Q_EMIT titleBarMoved(this->mapToGlobal(event->pos()));
}

void DockingPaneTitleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT titleBarEndMove(this->mapToGlobal(event->pos()));
        event->accept();
    }
}

void DockingPaneTitleWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setFocus();
        event->accept();
        Q_EMIT titleBarStartMove(this->mapToGlobal(event->pos()));
    }
}

void DockingPaneTitleWidget::setActive(bool active)
{
    m_active = active;
    update();
}

void DockingPaneTitleWidget::onFocusChanged(QWidget*, QWidget *now)
{
    setActive(this->isAncestorOf(now));
}



