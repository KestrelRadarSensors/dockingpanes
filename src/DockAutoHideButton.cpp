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
#include <QFont>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QTimer>

#include "DockAutoHideButton.h"

DockAutoHideButton::DockAutoHideButton(DockAutoHideButton::Position pos, QWidget* parent)
: QPushButton(parent), m_pos(pos)
{
    init();
}

DockAutoHideButton::DockAutoHideButton(const QString& text, QWidget* parent)
: QPushButton(text, parent)
{
    init();
}

DockAutoHideButton::DockAutoHideButton(const QIcon& icon, const QString& text, QWidget* parent)
: QPushButton(icon, text, parent)
{
    init();
}

void DockAutoHideButton::init()
{
    m_orientation = Qt::Horizontal;
    m_mirrored = false;
    m_swapDirection = false;

    this->setFont(QFont("Segoe UI", 9));

    this->setFocusPolicy(Qt::NoFocus);

    setMouseTracking(true);

    m_hovered = false;

    m_hoverTimer = new QTimer(this);

    m_hoverTimer->setInterval(1000);

    connect(m_hoverTimer,&QTimer::timeout, this, &DockAutoHideButton::onTimerElapsed);
}

Qt::Orientation DockAutoHideButton::orientation() const
{
    return m_orientation;
}

void DockAutoHideButton::swapDirection(bool state)
{
    m_swapDirection = state;
}

void DockAutoHideButton::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;

    switch (orientation) {
        case Qt::Horizontal: {
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            break;
        }

        case Qt::Vertical: {
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
            break;
        }
    }
}

bool DockAutoHideButton::mirrored() const
{
    return m_mirrored;
}

void DockAutoHideButton::setMirrored(bool mirrored)
{
    m_mirrored = mirrored;
}

QSize DockAutoHideButton::sizeHint() const
{
    QSize size;
    QFontMetrics fm(this->font());

    size.setWidth(fm.width(this->text()));
    size.setHeight(60);

    if (m_orientation == Qt::Vertical) {
        size.transpose();
    }

    return size;
}

void DockAutoHideButton::enterEvent(QEvent *event)
{
    m_hoverTimer->start();

    m_hovered = true;

    update();

    QWidget::enterEvent(event);
}

void DockAutoHideButton::leaveEvent(QEvent *event)
{
    m_hoverTimer->stop();

    m_hovered = false;

    update();

    QWidget::leaveEvent(event);
}

void DockAutoHideButton::onTimerElapsed(void)
{
    if (this->rect().contains(mapFromGlobal(QCursor::pos()))) {
        QTimer *timer = qobject_cast<QTimer *>(this->sender());

        timer->stop();

        Q_EMIT openFlyout();
    }
}

void DockAutoHideButton::paintEvent(QPaintEvent*)
{
    QStylePainter p(this);
    QColor color, textColor;

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::HighQualityAntialiasing, true);

    if (m_hovered) {
        color = textColor = QColor(0, 122, 204);
    } else {
        textColor = Qt::black;
        color = QColor(0xcc, 0xce, 0xdb);
    }

    p.setPen(textColor);

    switch (m_orientation) {
        case Qt::Horizontal: {
            if (m_mirrored) {
                p.rotate(180);
                p.translate(-width(), -height());
            }

            if (m_swapDirection) {
                p.fillRect(0, height()-6, width(), 6, color);
                p.drawText(0, 0, width(), height()-10, 0, this->text());
            }
            else {
                p.fillRect(0, 0, width(), 6, color);
                p.drawText(0, 10, width(), height(), 0, this->text());

            }

            break;
        }

        case Qt::Vertical: {
            if (m_mirrored) {
                p.rotate(-90);
                p.translate(-height(), 0);
            } else {
                p.rotate(90);
                p.translate(0, -width());
            }

            if (m_swapDirection) {
                p.fillRect(0, 0, height(), 6, color);
                p.drawText(0, 10, height(), width(), 0, this->text());
            } else {
                p.fillRect(0, width()-6, height(), 6, color);
                p.drawText(0, 0, height(), width()-10, 0, this->text());
            }

            break;
        }
    }
}

QStyleOptionButton* DockAutoHideButton::getStyleOption() const
{
    QStyleOptionButton* opt = new QStyleOptionButton();

    opt->initFrom(this);

    if (m_orientation == Qt::Vertical) {
        QSize size = opt->rect.size();

        size.transpose();

        opt->rect.setSize(size);
    }

    opt->features = QStyleOptionButton::None;
    opt->features |= QStyleOptionButton::Flat;
    opt->text = text();
    opt->icon = icon();
    opt->iconSize = iconSize();

    return opt;
}

void DockAutoHideButton::setPane(DockingPaneContainer *container, DockingPaneBase *pane)
{
    m_paneContainer = container;
    m_dockingPane = pane;
}

DockingPaneBase *DockAutoHideButton::pane(void)
{
    return(m_dockingPane);
}

DockingPaneContainer *DockAutoHideButton::container(void)
{
    return(m_paneContainer);
}

DockAutoHideButton::Position DockAutoHideButton::position(void)
{
    return(m_pos);
}

