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

#include <math.h>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "DockingPaneContainer.h"
#include "DockingPaneFlyoutWidget.h"
#include "DockingPaneGlow.h"
#include "DockingPaneManager.h"
#include "DockingPaneTitleWidget.h"
#include "DockingToolButton.h"

DockingPaneContainer::DockingPaneContainer(QWidget *parent) :
    DockingPaneBase(parent)
{

}

DockingPaneContainer::DockingPaneContainer(QString title, QString id, QWidget *parent, QWidget *clientWidget) :
    DockingPaneBase(parent)
{
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;

    vLayout = new QVBoxLayout();

    m_isActive = false;
    m_flyoutWidget = NULL;

    m_floatingGlow = NULL;

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    m_headerWidget = new QWidget();

    m_headerWidget->setAccessibleName("headerWidget");
    m_headerWidget->setObjectName("headerWidget");

    m_clientWidget = clientWidget;

    hLayout = new QHBoxLayout();

    m_titleWidget = new DockingPaneTitleWidget("Widget");

    connect(m_titleWidget, SIGNAL(titleBarStartMove(QPoint)), this, SLOT(onStartDragTitle(QPoint)));
    connect(m_titleWidget, SIGNAL(titleBarEndMove(QPoint)), this, SLOT(onEndDragTitle(QPoint)));
    connect(m_titleWidget, SIGNAL(titleBarMoved(QPoint)), this, SLOT(onMoveDragTitle(QPoint)));

    m_titleWidget->setFocusProxy(clientWidget);

    hLayout->addWidget(m_titleWidget);

    m_closeButton = new DockingToolButton(DockingToolButton::closeButtonInactive);
    m_pinButton = new DockingToolButton(DockingToolButton::pinButtonInactive);

    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(m_pinButton, SIGNAL(clicked()), this, SLOT(onPinButtonClicked()));

    m_closeButton->setMaximumWidth(16);
    m_pinButton->setMaximumWidth(16);

    hLayout->addWidget(m_pinButton);
    hLayout->addWidget(m_closeButton);
    hLayout->addSpacerItem(new QSpacerItem(2,0, QSizePolicy::Fixed));

    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    m_headerWidget->setLayout(hLayout);
    m_headerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_headerWidget->setMaximumHeight(20);

    vLayout->addWidget(m_headerWidget);

    m_clientLayout = new QGridLayout();

    m_clientLayout->setMargin(0);
    m_clientLayout->setVerticalSpacing(0);

    m_clientLayout->addWidget(clientWidget);

    vLayout->addLayout(m_clientLayout);

    vLayout->setContentsMargins(1, 1, 1, 1);
    vLayout->setSpacing(0);

    this->setLayout(vLayout);

    setName(title);
    setId(id);

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget *,QWidget*)));
}

void DockingPaneContainer::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);

    QPen pen(QColor(0xcc, 0xce, 0xdb));

    pen.setWidth(1);

    p.setPen(pen);

    QRect clientRect = this->rect();

    if (state()==DockingPaneBase::Floating) {
        p.setPen(QColor(0xaa, 0xaa, 0xaa));

        p.drawLine(clientRect.topLeft(), clientRect.topRight());
        p.drawLine(clientRect.topLeft(), clientRect.bottomLeft());
        p.drawLine(clientRect.topRight(), clientRect.bottomRight());
        p.drawLine(clientRect.bottomLeft(), clientRect.bottomRight());

        clientRect.adjust(1,1, -1,-1);
    }

    p.drawLine(clientRect.topLeft(), clientRect.topRight());
    p.drawLine(clientRect.topLeft(), clientRect.bottomLeft());
    p.drawLine(clientRect.topRight(), clientRect.bottomRight());
    p.drawLine(clientRect.bottomLeft(), clientRect.bottomRight());
}

void DockingPaneContainer::onCloseButtonClicked(void)
{
    m_dockingManager->closePane(this);
}

void DockingPaneContainer::onPinButtonClicked(void)
{
    m_dockingManager->hidePane(this);
}

void DockingPaneContainer::setName(QString name)
{
    m_titleWidget->setText(name);

    DockingPaneBase::setName(name);
}

void DockingPaneContainer::onFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old);

    this->setActivePane(this->isAncestorOf(now));
}

void DockingPaneContainer::setActivePane(bool active)
{
    m_isActive = active;

    m_titleWidget->setActive(m_isActive);

    if (m_isActive) {
        this->setStyleSheet("QWidget#headerWidget{background-color: #007acc}");

        this->m_pinButton->setButton(DockingToolButton::pinButtonActive);
        this->m_closeButton->setButton(DockingToolButton::closeButtonActive);

        if (m_floatingGlow) {
            m_floatingGlow->raise();
        }
    } else {
        this->setStyleSheet("QWidget#headerWidget{background-color: #eeeef2}");
        this->m_pinButton->setButton(DockingToolButton::pinButtonInactive);
        this->m_closeButton->setButton(DockingToolButton::closeButtonInactive);
    }

    update();
}

void DockingPaneContainer::floatPane(QRect rect)
{
    Q_UNUSED(rect);

    this->setParent(dockingManager()->mainWindow());

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    setState(DockingPaneBase::Floating);

    if (m_floatingGlow) {
        delete m_floatingGlow;
    }

    m_floatingGlow = new DockingPaneGlow(this, dockingManager()->mainWindow());
}

void DockingPaneContainer::floatPane(QPoint pos)
{
    QRect paneRect;

    paneRect.setTopLeft(mapToGlobal(QPoint(0, 0)));
    paneRect.setBottomRight(mapToGlobal(QPoint(width(), height())));

    m_dockingManager->closePane(this);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    paneRect.translate(pos);

    this->move(paneRect.topLeft());

    floatPane(paneRect);

    this->setParent(dockingManager()->mainWindow());

    this->show();

    qApp->setActiveWindow(this);
}

void DockingPaneContainer::saveLayout(QDomNode *parentNode, bool includeGeometry)
{
    QDomDocument doc = parentNode->ownerDocument();

    QDomElement domElement = doc.createElement(this->metaObject()->className());

    domElement.setAttribute("id", this->id());

    if (includeGeometry) {
        domElement.setAttribute("geometry", (QString) this->saveGeometry().toBase64());
    }

    parentNode->appendChild(domElement);
}

QWidget *DockingPaneContainer::clientWidget(void)
{
    return(m_clientWidget);
}

void DockingPaneContainer::setClientWidget(QWidget *widget)
{
    while(m_clientLayout->count()) {
        m_clientLayout->takeAt(0);
    }

    m_clientLayout->addWidget(widget);

    widget->setVisible(true);
}

int DockingPaneContainer::getPaneCount(void)
{
    return(1);
}

DockingPaneContainer *DockingPaneContainer::getPane(int index)
{
    Q_UNUSED(index);

    return(this);
}

void DockingPaneContainer::onUnpinContainer(void)
{
    m_flyoutWidget->restorePaneWidget();

    dockingManager()->unpinPane(m_flyoutWidget->pane());

    m_flyoutWidget = NULL;
}

void DockingPaneContainer::onCloseContainer(void)
{
    m_flyoutWidget->restorePaneWidget();

    dockingManager()->closePinnedPane(m_flyoutWidget->pane());

    m_flyoutWidget = NULL;
}

DockingPaneFlyoutWidget *DockingPaneContainer::openFlyout(bool hasFocus, QWidget *parent, FlyoutPosition pos, DockingPaneContainer *pane)
{
    m_flyoutWidget = new DockingPaneFlyoutWidget(hasFocus, parent->rect(), pane, pane, (DockingPaneFlyoutWidget::FlyoutPosition) pos, m_clientWidget, parent);

    connect(m_flyoutWidget, SIGNAL(unpinContainer()), this, SLOT(onUnpinContainer()));
    connect(m_flyoutWidget, SIGNAL(closeContainer()), this, SLOT(onCloseContainer()));
    connect(m_flyoutWidget, SIGNAL(startDragFlyoutTitle(QPoint)), this, SLOT(onStartDragFlyoutTitle(QPoint)));
    connect(m_flyoutWidget, SIGNAL(endDragFlyoutTitle(QPoint)), this, SLOT(onEndDragFlyoutTitle(QPoint)));
    connect(m_flyoutWidget, SIGNAL(moveDragFlyoutTitle(QPoint)), this, SLOT(onMoveDragFlyoutTitle(QPoint)));

    m_flyoutWidget->show();

    return(m_flyoutWidget);
}

void DockingPaneContainer::onStartDragTitle(QPoint pos)
{
    m_dockingManager->floatingPaneStartMove(this, pos);

    m_initialPos = pos;
}

void DockingPaneContainer::onEndDragTitle(QPoint pos)
{
    m_dockingManager->floatingPaneEndMove(this, pos);
}

void DockingPaneContainer::onMoveDragTitle(QPoint pos)
{
    QPoint deltaPos = pos-m_initialPos;

    if (state()==DockingPaneBase::Floating) {
        move(this->pos()+deltaPos);

        if (m_floatingGlow) {
            m_floatingGlow->update();
        }

        m_initialPos = pos;

        m_dockingManager->floatingPaneMoved(this, pos);
    } else {
        double trueLength = sqrt(pow(deltaPos.x(), 2) + pow(deltaPos.y(), 2));

        if (trueLength>5) {
            floatPane(deltaPos);

            m_initialPos = pos;

            m_dockingManager->floatingPaneStartMove(this, pos);
        }
    }
}

void DockingPaneContainer::onStartDragFlyoutTitle(QPoint pos)
{
    dockingManager()->floatingPaneStartMove(this, pos);

    m_initialPos = pos;
    m_draggingFlyout = false;
}

void DockingPaneContainer::onEndDragFlyoutTitle(QPoint pos)
{
    dockingManager()->floatingPaneEndMove(this, pos);

    m_flyoutWidget->endDrag();

    m_flyoutWidget = NULL;
}

void DockingPaneContainer::onMoveDragFlyoutTitle(QPoint pos)
{
    QPoint deltaPos = pos-m_initialPos;

    if (m_draggingFlyout) {
        move(this->pos()+deltaPos);

        if (m_floatingGlow) {
            m_floatingGlow->update();
        }

        m_initialPos = pos;

        dockingManager()->floatingPaneMoved(this, pos);
    } else {
        double trueLength = sqrt(pow(deltaPos.x(), 2) + pow(deltaPos.y(), 2));

        if (trueLength>5) {
            QWidget *widget = m_flyoutWidget->clientWidget();
            QSize size = m_flyoutWidget->paneRect().size();
            m_initialPos = pos;
            deltaPos = m_flyoutWidget->mapFromGlobal(m_initialPos);

            QPoint pos =m_flyoutWidget->mapToGlobal(m_flyoutWidget->paneRect().topLeft());

            m_flyoutWidget->beginDrag();

            setClientWidget(widget);

            dockingManager()->removePinnedButton(this);

            resize(size);

            floatPane(deltaPos);

            move(pos);
            dockingManager()->floatingPaneStartMove(this, pos);

            m_draggingFlyout = true;
        }
    }
}

QSize DockingPaneContainer::flyoutSize(void)
{
    if (m_flyoutSize.isValid()) {
        return(m_flyoutSize);
    }

    return(QSize(100, 100));
}

void DockingPaneContainer::setFlyoutSize(QSize flyoutSize)
{
    m_flyoutSize = flyoutSize;
}

void DockingPaneContainer::onFClicked(void)
{
    dockingManager()->dumpPaneList();
}

void DockingPaneContainer::setState(DockingPaneBase::State state)
{
    if (state!=DockingPaneBase::Floating) {
        m_pinButton->show();

        if (m_floatingGlow) {
            delete m_floatingGlow;

            m_floatingGlow = NULL;
        }
    } else {
        m_pinButton->hide();
    }

    DockingPaneBase::setState(state);
}

DockingPaneGlow *DockingPaneContainer::floatingGlow(void)
{
    return(m_floatingGlow);
}
