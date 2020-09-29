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
#include <QGridLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#include "DockingPaneContainer.h"
#include "DockingPaneFlyoutWidget.h"
#include "DockingPaneTitleWidget.h"
#include "DockingToolButton.h"

DockingPaneFlyoutWidget::DockingPaneFlyoutWidget(bool hasFocus, DockingPaneContainer *container, DockingPaneContainer *pane, FlyoutPosition pos, QWidget *widget, QWidget *parent) :
    QWidget(parent),
    m_pane(pane),
    m_container(container),
    m_clientWidget(widget),
    m_pos(pos),
    m_isActive(false),
    m_dragMode(false),
    m_resizeMode(false)
{
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;

    vLayout = new QVBoxLayout();

    setAutoFillBackground(true);

    if (!hasFocus) {
        setAttribute(Qt::WA_ShowWithoutActivating);

        QTimer::singleShot(1000, this, &DockingPaneFlyoutWidget::autoHideTimeout);
    }

    m_headerWidget = new QWidget();
    m_headerWidget->setAccessibleName("headerWidget");
    m_headerWidget->setObjectName("headerWidget");
    m_headerWidget->setMinimumHeight(6+m_headerWidget->fontMetrics().height());
    m_headerWidget->setMaximumHeight(6+m_headerWidget->fontMetrics().height());

    hLayout = new QHBoxLayout();

    m_titleWidget = new DockingPaneTitleWidget(m_pane->name());
    m_titleWidget->setFocusProxy(widget);
    connect(m_titleWidget, &DockingPaneTitleWidget::titleBarStartMove, this, &DockingPaneFlyoutWidget::startDragFlyoutTitle);
    connect(m_titleWidget, &DockingPaneTitleWidget::titleBarEndMove, this, &DockingPaneFlyoutWidget::endDragFlyoutTitle);
    connect(m_titleWidget, &DockingPaneTitleWidget::titleBarMoved, this, &DockingPaneFlyoutWidget::moveDragFlyoutTitle);
    hLayout->addWidget(m_titleWidget);

    m_closeButton = new DockingToolButton(DockingToolButton::closeButtonInactive);
    m_pinButton = new DockingToolButton(DockingToolButton::unpinButtonInactive);
    m_closeButton->setMaximumWidth(16);
    m_pinButton->setMaximumWidth(16);
    connect(m_closeButton, &DockingToolButton::clicked, this, &DockingPaneFlyoutWidget::closeContainer);
    connect(m_pinButton, &DockingToolButton::clicked, this, &DockingPaneFlyoutWidget::unpinContainer);
    hLayout->addWidget(m_pinButton);
    hLayout->addWidget(m_closeButton);

    hLayout->addSpacerItem(new QSpacerItem(2,0, QSizePolicy::Fixed));
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    m_headerWidget->setLayout(hLayout);
    m_headerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    vLayout->addWidget(m_headerWidget);

    m_clientLayout = new QGridLayout();
    m_clientLayout->setMargin(0);
    m_clientLayout->setVerticalSpacing(0);
    m_clientLayout->addWidget(widget);

    vLayout->addLayout(m_clientLayout);
    vLayout->setSpacing(0);
    this->setLayout(vLayout);

    m_clientWidget->show();

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    switch(pos) {
        case Left: {
            vLayout->setContentsMargins(1, 1, 6, 1);
            m_size = m_container->flyoutSize().width();
            this->setMinimumWidth(50);
            break;
        }
        case Right: {
            vLayout->setContentsMargins(6, 1, 1, 1);
            m_size = m_container->flyoutSize().width();
            this->setMinimumWidth(50);
            break;
        }
        case Top: {
            vLayout->setContentsMargins(1, 1, 1, 6);
            m_size = m_container->flyoutSize().height();
            this->setMinimumHeight(50);
            break;
        }
        case Bottom: {
            vLayout->setContentsMargins(1, 6, 1, 1);
            m_size = m_container->flyoutSize().height();
            this->setMinimumHeight(50);
            break;
        }
    }

    // @todo Investigate why this syntax makes the program crash when closing the pane
    // connect(qApp, &QApplication::focusChanged, this, &DockingPaneFlyoutWidget::onFocusChanged);
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(onFocusChanged(QWidget*, QWidget*)));

    if (hasFocus) {
        m_clientWidget->setFocus();
    }

    qApp->installEventFilter(this);
    parent->installEventFilter(this);

    setPositionAndSize();
    setMouseTracking(true);
}


void DockingPaneFlyoutWidget::resizeEvent(QResizeEvent*)
{
    m_headerWidget->setMinimumHeight(6+m_headerWidget->fontMetrics().height());
    m_headerWidget->setMaximumHeight(6+m_headerWidget->fontMetrics().height());
}

void DockingPaneFlyoutWidget::enterEvent(QEvent*)
{
    updateCursor();
}

void DockingPaneFlyoutWidget::leaveEvent(QEvent*)
{
    this->unsetCursor();
}

void DockingPaneFlyoutWidget::updateCursor()
{
    QPoint pt = mapFromGlobal(QCursor::pos());

    switch(m_pos) {
        case Left: {
            if ((pt.x()>=this->width()-6)) {
                setCursor(Qt::SizeHorCursor);
            } else {
                unsetCursor();
            }
            break;
        }
        case Right: {
            if ((pt.x()>=0) && (pt.x()<=6)) {
                setCursor(Qt::SizeHorCursor);
            } else {
                unsetCursor();
            }
            break;
        }
        case Top: {
            if ((pt.y()>=this->height()-6)) {
                setCursor(Qt::SizeVerCursor);
            } else {
                unsetCursor();
            }
            break;
        }
        case Bottom: {
            if ((pt.y()>=0) && (pt.y()<=6)) {
                setCursor(Qt::SizeVerCursor);
            } else {
                unsetCursor();
            }
            break;
        }
    }
}

void DockingPaneFlyoutWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    QPen pen(QColor(0xcc, 0xce, 0xdb));

    pen.setWidth(1);

    p.setPen(pen);

    QRect rc = paneRect();

    p.drawLine(rc.topLeft(), rc.topRight());
    p.drawLine(rc.topLeft(), rc.bottomLeft());
    p.drawLine(rc.topRight(), rc.bottomRight());
    p.drawLine(rc.bottomLeft(), rc.bottomRight());
}

QRect DockingPaneFlyoutWidget::paneRect(void)
{
    QRect rc = this->rect();
    switch(m_pos) {
        case Left: {
            rc.adjust(0,0,-5,0);
            break;
        }
        case Right: {
            rc.adjust(5,0,0,0);
            break;
        }
        case Bottom: {
            rc.adjust(0,5,0,0);
            break;
        }
        case Top: {
            rc.adjust(0,0,0,-5);
            break;
        }
    }
    return(rc);
}

void DockingPaneFlyoutWidget::restorePaneWidget()
{
    if (!this->m_dragMode) {
        m_container->setClientWidget(m_clientWidget);
    }
}

DockingPaneContainer *DockingPaneFlyoutWidget::pane(void)
{
    return(m_pane);
}

void DockingPaneFlyoutWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = mapToGlobal(event->pos())-m_initialPos;
    QSize flyoutSize = m_container->flyoutSize();

    updateCursor();

    if (m_resizeMode) {
        switch(m_pos) {
            case Left: {
                m_size += delta.x();
                flyoutSize.setWidth(m_size);
                break;
            }
            case Right: {
                m_size -= delta.x();
                flyoutSize.setWidth(m_size);
                break;
            }
            case Bottom: {
                m_size -= delta.y();
                flyoutSize.setHeight(m_size);
                break;
            }
            case Top: {
                m_size += delta.y();
                flyoutSize.setHeight(m_size);
                break;
            }
        }
        m_container->setFlyoutSize(flyoutSize);
        m_initialPos += delta;
        setPositionAndSize();
    }
}

void DockingPaneFlyoutWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button()==Qt::LeftButton) {
        releaseMouse();
        m_resizeMode = false;
        event->accept();
    }
}

void DockingPaneFlyoutWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button()==Qt::LeftButton) {
        m_initialPos = mapToGlobal(event->pos());
        m_resizeMode = true;
        grabMouse();
        event->accept();
    }
}

void DockingPaneFlyoutWidget::setActivePane(bool active)
{
    m_isActive = active;
    m_titleWidget->setActive(m_isActive);

    if (m_isActive) {
        this->setStyleSheet("QWidget#headerWidget{background-color: #007acc}");
        this->m_pinButton->setButton(DockingToolButton::unpinButtonActive);
        this->m_closeButton->setButton(DockingToolButton::closeButtonActive);
    } else {
        this->setStyleSheet("QWidget#headerWidget{background-color: #eeeef2}");
        this->m_pinButton->setButton(DockingToolButton::unpinButtonInactive);
        this->m_closeButton->setButton(DockingToolButton::closeButtonInactive);
    }

    update();
}

void DockingPaneFlyoutWidget::closeEvent(QCloseEvent* event)
{
    restorePaneWidget();
    event->accept();
}

void DockingPaneFlyoutWidget::onFocusChanged(QWidget*, QWidget *now)
{
    setActivePane(this->isAncestorOf(now));
    if (!m_dragMode) {
        if (!this->isAncestorOf(now)) {
            Q_EMIT flyoutFocusLost();
        }
    }
}

bool DockingPaneFlyoutWidget::eventFilter(QObject* obj, QEvent* event)
{
    switch(event->type()) {
        case QEvent::MouseMove: {
            if (this->underMouse()) {
                updateCursor();
            }
            break;
        }
        case QEvent::Resize: {
            if (obj == this->parent()) {
                setPositionAndSize();
            }
            break;
        }
        default: {
            break;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void DockingPaneFlyoutWidget::setPositionAndSize(void)
{
    QRect rc = parentWidget()->rect();

    switch(m_pos) {
        case Right: {
            rc.setLeft(rc.right()-qMax(qMin(m_size, rc.width()-50), this->minimumWidth()));
            rc.adjust(-9,5,-5,-5);
            break;
        }
        case Left: {
            rc.setRight(rc.left()+qMin(qMax(m_size, this->minimumWidth()), rc.width()-50));
            rc.adjust(5,5,9,-5);
            break;
        }
        case Bottom: {
            rc.setTop(rc.bottom()-qMax(qMin(m_size, rc.height()-50), this->minimumHeight()));
            rc.adjust(5,-9,-5,-5);
            break;
        }
        case Top: {
            rc.setBottom(rc.top()+qMax(qMin(m_size, rc.height()-50), this->minimumHeight()));
            rc.adjust(5,5,-5,-9);
            break;
        }
    }

    QSize size;
    size = rc.size();
    move(rc.topLeft());
    resize(size);
}

void DockingPaneFlyoutWidget::autoHideTimeout(void)
{
    if (!m_isActive) {
        Q_EMIT autoHideFlyout();
    }
}

void DockingPaneFlyoutWidget::beginDrag(void)
{
    this->m_dragMode = true;
    this->hide();
    restorePaneWidget();
}

void DockingPaneFlyoutWidget::endDrag(void)
{
    Q_EMIT flyoutFocusLost();
}

QWidget *DockingPaneFlyoutWidget::clientWidget(void)
{
    return(m_clientWidget);
}
