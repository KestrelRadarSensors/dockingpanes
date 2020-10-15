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
#include <QColor>
#include <QDebug>
#include <QDomDocument>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QUuid>
#include <QVBoxLayout>

#include "DockingPaneFlyoutWidget.h"
#include "DockingPaneGlow.h"
#include "DockingPaneManager.h"
#include "DockingPaneTabbedContainer.h"
#include "DockingPaneTitleWidget.h"
#include "DockingToolButton.h"

DockingPaneTabbedContainer::DockingPaneTabbedContainer(QWidget *parent) :
    DockingPaneContainer(parent)
{
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;

    m_draggedPane = nullptr;
    m_floatingGlow = nullptr;

    vLayout = new QVBoxLayout();

    m_isActive = false;

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    m_headerWidget = new QWidget();

    m_headerWidget->setAccessibleName("headerWidget");
    m_headerWidget->setObjectName("headerWidget");

    hLayout = new QHBoxLayout();

    m_titleWidget = new DockingPaneTitleWidget("Widget");

    connect(m_titleWidget, &DockingPaneTitleWidget::titleBarStartMove, this, &DockingPaneTabbedContainer::onStartDragTitle);
    connect(m_titleWidget, &DockingPaneTitleWidget::titleBarEndMove, this, &DockingPaneTabbedContainer::onEndDragTitle);
    connect(m_titleWidget, &DockingPaneTitleWidget::titleBarMoved, this, &DockingPaneTabbedContainer::onMoveDragTitle);

    this->setFocusPolicy(Qt::StrongFocus);

    hLayout->addWidget(m_titleWidget);

    m_closeButton = new DockingToolButton(DockingToolButton::closeButtonInactive);
    m_pinButton = new DockingToolButton(DockingToolButton::pinButtonInactive);

    connect(m_closeButton, &DockingToolButton::clicked, this, &DockingPaneTabbedContainer::onCloseButtonClicked);
    connect(m_pinButton, &DockingToolButton::clicked, this, &DockingPaneTabbedContainer::onPinButtonClicked);

    m_closeButton->setMaximumWidth(16);
    m_pinButton->setMaximumWidth(16);

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

    m_stackedWidget = new QStackedWidget();

    m_clientWidget = m_stackedWidget;

    m_clientLayout->addWidget(m_stackedWidget);

    vLayout->addLayout(m_clientLayout);

    vLayout->setSpacing(0);

    this->setLayout(vLayout);

    this->setFont(QFont("Segoe UI", 9));

    updateMargins();

    connect(qApp, &QApplication::focusChanged, this, &DockingPaneTabbedContainer::onFocusChanged);

    m_stackedWidget->setFocusPolicy(Qt::StrongFocus);

    m_id = QUuid::createUuid().toString();

    m_fromMousePressEvent = false;
}

DockingPaneTabbedContainer::~DockingPaneTabbedContainer()
{
}

void DockingPaneTabbedContainer::onPinButtonClicked(void)
{
    m_dockingManager->hidePane(this);
}


void DockingPaneTabbedContainer::restoreChildWidgets(void)
{
    int i;
    QList<QWidget *> widgets;

    for (i=0;i<m_stackedWidget->count();i++) {
        widgets.append(m_stackedWidget->widget(i));
    }

    for (i=0;i<m_paneList.count();i++) {
        m_paneList.at(i)->setClientWidget(widgets.at(i));
    }
}

bool DockingPaneTabbedContainer::addPane(DockingPaneContainer *child)
{
    if (DockingPaneTabbedContainer *tabbedPane = qobject_cast<DockingPaneTabbedContainer *>(child)) {
        int i;
        QList<DockingPaneContainer *> paneList;

        tabbedPane->restoreChildWidgets();

        for (i=0;i<tabbedPane->m_paneList.count();i++) {
            paneList.append(tabbedPane->m_paneList.at(i));
        }

        for (i=0;i<paneList.count();i++) {
            addPane(paneList.at(i));
        }

        tabbedPane->m_paneList.clear();

        return(true);
    }

    m_paneList.append(child);

    m_stackedWidget->addWidget(child->clientWidget());

    m_stackedWidget->setCurrentWidget(child->clientWidget());

    setName(child->name());

    updateMargins();

    update();

    return(false);
}

void DockingPaneTabbedContainer::resizeEvent(QResizeEvent*)
{
    updateMargins();
}

void DockingPaneTabbedContainer::paintEvent(QPaintEvent*)
{
    int i;

    QPainter p(this);

    QPen pen(QColor(0xcc, 0xce, 0xdb));

    calculateButtonsRectangles();

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

    QRect rc = clientRect;

    if (m_paneList.count()>1) {
        rc.adjust(0,0,0, -(5+this->fontMetrics().height()));
    }

    p.drawLine(rc.topLeft(), rc.topRight());
    p.drawLine(rc.topLeft(), rc.bottomLeft());
    p.drawLine(rc.topRight(), rc.bottomRight());

    if (m_paneList.count()<=1) {
        p.drawLine(rc.bottomLeft(), rc.bottomRight());

        return;
    }

    QFont f = this->font();

    p.setFont(f);

    QFontMetrics fm(f);

    int x = 0;

    QRect tr = rc;

    for(i=0;i<m_tabWidths.count();i++) {
        p.setRenderHint(QPainter::HighQualityAntialiasing, true);

        tr.setLeft(x);
        tr.setTop(rc.bottom());
        tr.setRight(x+m_tabWidths.at(i));
        tr.setBottom(clientRect.bottom());

        p.setPen(Qt::black);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawText(tr, Qt::AlignHCenter | Qt::AlignVCenter, fm.elidedText(m_paneList.at(i)->name(), Qt::ElideRight, tr.width()));

        p.setRenderHint(QPainter::Antialiasing, false);
        p.setPen(pen);

        if (m_stackedWidget->currentIndex()==i) {
            p.drawLine(tr.topLeft(), tr.bottomLeft());
            p.drawLine(tr.bottomLeft(), tr.bottomRight());
            p.drawLine(tr.bottomRight(), tr.topRight());
        } else {
            p.drawLine(tr.topLeft(), tr.topRight());
        }

        x += m_tabWidths.at(i);
    }

    p.drawLine(tr.topRight(), rc.bottomRight());
}

void DockingPaneTabbedContainer::calculateButtonsRectangles(void)
{
    int availableWidth, currentIndex;
    int tabGap = 5, i;

    QFontMetrics fm = this->fontMetrics();

    m_tabWidths.clear();

    availableWidth = this->rect().width();

    currentIndex = m_stackedWidget->currentIndex();

    if (currentIndex==0) {
        int selectedWidth = (fm.width(m_paneList.at(currentIndex)->name())+(tabGap*2));
        int spaceAfter = availableWidth-selectedWidth;
        int requiredSpace = 0;

        for (i=1;i<m_paneList.count();i++) {
            requiredSpace += fm.width(m_paneList.at(i)->name())+(tabGap*2);
        }

        m_tabWidths.append(selectedWidth);

        if (requiredSpace>spaceAfter) {
            int itemSpace;

            if (m_paneList.count()>1) {
                itemSpace = spaceAfter/(m_paneList.count()-1);

                for(i=1;i<m_paneList.count();i++) {
                    m_tabWidths.append(itemSpace);
                }
            }
        } else {
            for(i=1;i<m_paneList.count();i++) {
                m_tabWidths.append(fm.width(m_paneList.at(i)->name())+(tabGap*2));
            }
        }

        return;
    }

    if (m_stackedWidget->currentIndex()==m_stackedWidget->count()-1) {
        int selectedWidth = (fm.width(m_paneList.at(currentIndex)->name())+(tabGap*2));
        int spaceBefore = availableWidth-selectedWidth;
        int requiredSpace = 0;

        for (i=0;i<m_paneList.count()-1;i++) {
            requiredSpace += fm.width(m_paneList.at(i)->name())+(tabGap*2);
        }

        if (requiredSpace>spaceBefore) {
            int itemSpace;

            itemSpace = spaceBefore/(m_paneList.count()-1);

            for(i=0;i<m_paneList.count()-1;i++) {
                m_tabWidths.append(itemSpace);
            }
        } else {
            for(i=0;i<m_paneList.count()-1;i++) {
                m_tabWidths.append(fm.width(m_paneList.at(i)->name())+(tabGap*2));
            }
        }

        m_tabWidths.append(selectedWidth);

        return;
    }

    int selectedWidth = (fm.width(m_paneList.at(currentIndex)->name())+(tabGap*2));
    int spaceBeforeAfter = availableWidth-selectedWidth;
    int requiredSpace = 0;

    for (i=0;i<m_paneList.count();i++) {
        if (i!=currentIndex) {
            requiredSpace += fm.width(m_paneList.at(i)->name())+(tabGap*2);
        }
    }

    if (requiredSpace>spaceBeforeAfter)
    {
        int itemSpace;

        itemSpace = spaceBeforeAfter/(m_paneList.count()-1);

        for(i=0;i<m_paneList.count();i++) {
            if (i==currentIndex) {
                m_tabWidths.append(selectedWidth);
            } else {
                m_tabWidths.append(itemSpace);
            }
        }
    } else {
        for(i=0;i<m_paneList.count();i++) {
            if (i==currentIndex) {
                m_tabWidths.append(selectedWidth);
            } else {
                m_tabWidths.append(fm.width(m_paneList.at(i)->name())+(tabGap*2));
            }
        }
    }
}

void DockingPaneTabbedContainer::mousePressEvent(QMouseEvent *e)
{
    QRect buttonRect;
    int x = 0, i;

    if (e->button() == Qt::LeftButton) {
        calculateButtonsRectangles();

        buttonRect.setTop(this->rect().bottom()-(5+this->fontMetrics().height()));
        buttonRect.setBottom(this->rect().bottom());

        for (i=0;i<m_tabWidths.count();i++) {
            buttonRect.setLeft(x);
            buttonRect.setRight(x+m_tabWidths.at(i));

            if (buttonRect.contains(e->pos())) {
                m_fromMousePressEvent = true;

                e->accept();

                m_stackedWidget->setCurrentIndex(i);

                grabMouse();

                setName(m_paneList.at(i)->name());

                update();

                m_clientWidget->setFocus();

                m_invalidTabRect = QRect();

                m_originalClickPos = mapToGlobal(e->pos());

                return;
            }

            x += m_tabWidths.at(i);
        }
    }
}

void DockingPaneTabbedContainer::mouseReleaseEvent(QMouseEvent *e)
{
    m_fromMousePressEvent = false;

    if (e->button() == Qt::LeftButton) {
        if (m_draggedPane) {
            dockingManager()->floatingPaneEndMove(m_draggedPane, this->mapToGlobal(e->pos()));
        }

        m_draggedPane = nullptr;

        releaseMouse();

        e->accept();

        if (m_paneList.count()<=1) {
            restoreChildWidgets();

            if (state()==DockingPaneBase::Floating) {
                m_paneList.at(0)->move(this->pos());
                m_paneList.at(0)->resize(this->size());

                if (m_floatingGlow) {
                    delete m_floatingGlow;
                    m_floatingGlow = nullptr;
                }
            } else {
                dockingManager()->replacePane(this, m_paneList.at(0));
            }

            if (state()==DockingPaneBase::Floating) {
                m_paneList.at(0)->floatPane(m_paneList.at(0)->rect());
            }

            m_paneList.at(0)->show();

            if (m_paneList.at(0)->floatingGlow()) {
                m_paneList.at(0)->floatingGlow()->update();
            }

            m_paneList.clear();

            hide();

            dockingManager()->deletePane(this);
        }
    }
}

QRect DockingPaneTabbedContainer::getButtonRect(int pos)
{
    QRect buttonRect;
    int i;
    int x = 0;

    buttonRect.setTop(this->rect().bottom()-(5+this->fontMetrics().height()));
    buttonRect.setBottom(this->rect().bottom());

    for (i=0;i<m_tabWidths.count();i++) {
        buttonRect.setLeft(x);
        buttonRect.setRight(x+m_tabWidths.at(i));

        if (i==pos) {
            return(buttonRect);
        }

        x+= m_tabWidths.at(i);
    }

    return(QRect());
}

void DockingPaneTabbedContainer::mouseMoveEvent(QMouseEvent *e)
{
    QRect tabRect;

    if (!m_fromMousePressEvent) {
        return;
    }

    tabRect = this->rect();

    tabRect.setTop(this->rect().bottom()-(5+this->fontMetrics().height()));

    tabRect.adjust(-10, -10, 10, 10);

    if (!m_draggedPane) {
        if (!tabRect.contains(e->pos())) {
            QWidget *widget;

            QPoint deltaPos = mapFromGlobal(m_originalClickPos);

            QPoint pos = mapToGlobal(this->rect().topLeft());

            int currentIndex = m_stackedWidget->currentIndex();

            m_initialPos = this->mapToGlobal(e->pos());

            m_draggedPane = m_paneList.at(currentIndex);

            widget = m_stackedWidget->currentWidget();

            m_draggedPane->setClientWidget(widget);
            m_paneList.removeAt(currentIndex);
            m_stackedWidget->removeWidget(widget);

            updateMargins();

            update();

            setName(m_paneList.at(m_stackedWidget->currentIndex())->name());

            m_draggedPane->resize(this->size());

            m_draggedPane->move(pos);
            m_draggedPane->floatPane(QPoint(-deltaPos.x(), -10));

            m_initialPos = pos;

            dockingManager()->floatingPaneStartMove(m_draggedPane, m_initialPos);
        } else {
            QRect buttonRect;
            int i, x=0;

            calculateButtonsRectangles();

            buttonRect.setTop(this->rect().bottom()-(5+this->fontMetrics().height()));
            buttonRect.setBottom(this->rect().bottom());

            for (i=0;i<m_tabWidths.count();i++) {
                buttonRect.setLeft(x);
                buttonRect.setRight(x+m_tabWidths.at(i));

                buttonRect.adjust(1,0,0,-1);

                if (buttonRect.contains(e->pos())) {
                    if (i!=m_stackedWidget->currentIndex()) {
                        if (!m_invalidTabRect.isValid() || (m_invalidTabRect.isValid() && (!m_invalidTabRect.contains(e->pos())))) {
                            QRect newRect;
                            int oldIndex;

                            oldIndex = m_stackedWidget->currentIndex();

                            m_paneList.swap(m_stackedWidget->currentIndex(), i);
                            m_stackedWidget->insertWidget(i, m_stackedWidget->currentWidget());
                            m_stackedWidget->setCurrentIndex(i);

                            update();

                            calculateButtonsRectangles();

                            newRect = getButtonRect(i);

                            if (newRect.contains(e->pos())) {
                                m_invalidTabRect = QRect();
                            } else {
                                if (i<oldIndex) {
                                    m_invalidTabRect = getButtonRect(i+1);
                                    m_invalidTabRect.setLeft(newRect.right());
                                } else {
                                    m_invalidTabRect = getButtonRect(i-1);
                                    m_invalidTabRect.setRight(newRect.left());
                                }
                            }

                            return;
                        }
                    } else {
                        m_invalidTabRect = QRect();
                    }
                }

                x+= m_tabWidths.at(i);
            }
        }
    } else {
        QPoint newPos;

        newPos = this->mapToGlobal(e->pos());

        QPoint deltaPos = newPos-m_initialPos;

        m_draggedPane->move(m_draggedPane->pos()+deltaPos);

        if (m_draggedPane->floatingGlow()) {
            m_draggedPane->floatingGlow()->update();
        }

        m_initialPos = newPos;

        dockingManager()->floatingPaneMoved(m_draggedPane, newPos);
    }
}

int DockingPaneTabbedContainer::getPaneCount(void)
{
    return(m_paneList.count());
}

DockingPaneContainer *DockingPaneTabbedContainer::getPane(int index)
{
    return(m_paneList.at(index));
}

void DockingPaneTabbedContainer::onUnpinContainer(void)
{
    m_flyoutWidget->close();

    dockingManager()->unpinPane(this);

    m_flyoutWidget = nullptr;
}

void DockingPaneTabbedContainer::onCloseContainer(void)
{
    QWidget *clientWidget = m_flyoutWidget->clientWidget();
    DockingPaneContainer *pane;

    m_flyoutWidget->beginDrag();

    pane = m_flyoutWidget->pane();

    pane->setClientWidget(clientWidget);

    m_paneList.removeOne(pane);
    m_stackedWidget->removeWidget(pane->clientWidget());

    updateMargins();

    update();

    setName(m_paneList.at(m_stackedWidget->currentIndex())->name());

    dockingManager()->removePinnedButton(this, pane);

    if (m_paneList.count()<=1) {
        restoreChildWidgets();

        dockingManager()->updateAutohideButton(this, m_paneList.at(0), m_paneList.at(0), m_paneList.at(0));

        m_paneList.at(0)->setState(DockingPaneBase::Pinned);

        m_paneList.clear();

        hide();

        dockingManager()->deletePane(this);
    }

    m_flyoutWidget->endDrag();

    m_flyoutWidget = nullptr;
}


void DockingPaneTabbedContainer::onStartDragTitle(QPoint pos)
{
    m_dockingManager->floatingPaneStartMove(this, pos);

    m_initialPos = pos;
}

void DockingPaneTabbedContainer::onEndDragTitle(QPoint pos)
{
    m_dockingManager->floatingPaneEndMove(this, pos);
}

void DockingPaneTabbedContainer::onMoveDragTitle(QPoint pos)
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

DockingPaneFlyoutWidget *DockingPaneTabbedContainer::openFlyout(bool hasFocus, QWidget *parent, FlyoutPosition pos, DockingPaneContainer *pane)
{
    m_flyoutWidget = new DockingPaneFlyoutWidget(hasFocus, this, pane, (DockingPaneFlyoutWidget::FlyoutPosition) pos,  m_stackedWidget->widget(m_paneList.indexOf(pane)), parent);

    connect(m_flyoutWidget, &DockingPaneFlyoutWidget::unpinContainer, this, &DockingPaneTabbedContainer::onUnpinContainer);
    connect(m_flyoutWidget, &DockingPaneFlyoutWidget::closeContainer, this, &DockingPaneTabbedContainer::onCloseContainer);
    connect(m_flyoutWidget, &DockingPaneFlyoutWidget::startDragFlyoutTitle, this, &DockingPaneTabbedContainer::onStartDragFlyoutTitle);
    connect(m_flyoutWidget, &DockingPaneFlyoutWidget::endDragFlyoutTitle, this, &DockingPaneTabbedContainer::onEndDragFlyoutTitle);
    connect(m_flyoutWidget, &DockingPaneFlyoutWidget::moveDragFlyoutTitle, this, &DockingPaneTabbedContainer::onMoveDragFlyoutTitle);

    m_flyoutWidget->show();

    return(m_flyoutWidget);
}

void DockingPaneTabbedContainer::onStartDragFlyoutTitle(QPoint pos)
{
    m_draggedPane = nullptr;

    dockingManager()->floatingPaneStartMove(m_flyoutWidget->pane(), pos);

    m_initialPos = pos;

    m_draggingFlyout = false;
}

void DockingPaneTabbedContainer::onEndDragFlyoutTitle(QPoint pos)
{
    dockingManager()->floatingPaneEndMove(m_flyoutWidget->pane(), pos);

    if (m_paneList.count()<=1) {
        restoreChildWidgets();

        dockingManager()->updateAutohideButton(this, m_paneList.at(0), m_paneList.at(0), m_paneList.at(0));

        m_paneList.clear();

        hide();

        dockingManager()->deletePane(this);
    }

    m_draggedPane = nullptr;

    m_flyoutWidget->endDrag();

    m_flyoutWidget = nullptr;
}

void DockingPaneTabbedContainer::onMoveDragFlyoutTitle(QPoint pos)
{
    QPoint deltaPos = pos-m_initialPos;

    if (m_draggedPane) {
        m_draggedPane->move(m_draggedPane->pos()+deltaPos);

        if (m_draggedPane->floatingGlow()) {
            m_draggedPane->floatingGlow()->update();
        }

        m_initialPos = pos;

        dockingManager()->floatingPaneMoved(m_draggedPane, pos);
    } else {
        double trueLength = sqrt(pow(deltaPos.x(), 2) + pow(deltaPos.y(), 2));

        if (trueLength>5) {
            QWidget *clientWidget = m_flyoutWidget->clientWidget();
            QSize size = m_flyoutWidget->paneRect().size();

            deltaPos = m_flyoutWidget->mapFromGlobal(m_initialPos);

            QPoint pos = m_flyoutWidget->paneRect().topLeft();

            m_flyoutWidget->beginDrag();

            m_draggedPane = m_flyoutWidget->pane();

            m_draggedPane->setClientWidget(clientWidget);

            m_paneList.removeOne(m_draggedPane);
            m_stackedWidget->removeWidget(m_draggedPane->clientWidget());

            updateMargins();

            update();

            setName(m_paneList.at(m_stackedWidget->currentIndex())->name());

            dockingManager()->removePinnedButton(this, m_draggedPane);

            m_draggedPane->move(pos);

            m_draggedPane->floatPane(-deltaPos);

            m_draggedPane->resize(size);

            m_initialPos = pos;

            dockingManager()->floatingPaneStartMove(m_draggedPane, pos);
        }
    }
}

void DockingPaneTabbedContainer::onFocusChanged(QWidget*, QWidget *now)
{
    this->setActivePane(this->isAncestorOf(now));

    if (now==m_stackedWidget) {
        if (m_stackedWidget->currentWidget()) {
            m_stackedWidget->currentWidget()->setFocus();
        }
    }
}

void DockingPaneTabbedContainer::setClientWidget(QWidget *widget)
{
    int i = 0, insertIndex = -1;

    foreach(DockingPaneContainer *pane, m_paneList) {
        if (pane->clientWidget()==widget) {
            insertIndex = i;
            break;
        }

        i++;
    }

    if (insertIndex!=-1) {
        m_stackedWidget->insertWidget(insertIndex, widget);
    }
}

void DockingPaneTabbedContainer::updateMargins(void)
{
    int framePixels = 1;
    int tabSize = 0;

    if (m_paneList.count()>1) {
        tabSize = 5+this->fontMetrics().height();
    }

    if (state()==DockingPaneBase::Floating) {
        framePixels++;
    }

    this->layout()->setContentsMargins(framePixels, framePixels, framePixels, tabSize+framePixels);
}

void DockingPaneTabbedContainer::onCloseButtonClicked(void)
{
    QWidget *widget;
    DockingPaneContainer *pane;

    int currentIndex = m_stackedWidget->currentIndex();

    pane = m_paneList.at(currentIndex);

    widget = m_stackedWidget->currentWidget();

    pane->setClientWidget(widget);
    m_paneList.removeAt(currentIndex);
    m_stackedWidget->removeWidget(widget);

    updateMargins();

    update();

    setName(m_paneList.at(m_stackedWidget->currentIndex())->name());

    this->dockingManager()->closePane(pane);

    if (m_paneList.count()<=1) {
        restoreChildWidgets();

        if (state()==DockingPaneBase::Floating) {
            m_paneList.at(0)->move(this->pos());
            m_paneList.at(0)->resize(this->size());

            if (m_floatingGlow) {
                delete m_floatingGlow;
                m_floatingGlow = nullptr;
            }
        } else {
            dockingManager()->replacePane(this, m_paneList.at(0));
        }

        if (state()==DockingPaneBase::Floating) {
            m_paneList.at(0)->floatPane(m_paneList.at(0)->rect());
        }

        m_paneList.at(0)->show();

        if (m_paneList.at(0)->floatingGlow()) {
            m_paneList.at(0)->floatingGlow()->update();
        }

        m_paneList.clear();

        hide();

        dockingManager()->deletePane(this);
    }
}

void DockingPaneTabbedContainer::saveLayout(QDomNode *parentNode, bool includeGeometry)
{
    QDomDocument doc = parentNode->ownerDocument();

    QDomElement domElement = doc.createElement(this->metaObject()->className());

    domElement.setAttribute("id", this->id());
    domElement.setAttribute("selectedTab", this->m_paneList[this->m_stackedWidget->currentIndex()]->id());

    if (includeGeometry) {
        domElement.setAttribute("geometry", (QString) this->saveGeometry().toBase64());
    }

    QDomElement panesElement = doc.createElement("panes");

    domElement.appendChild(panesElement);

    foreach(DockingPaneContainer *pane, m_paneList) {
        QDomElement paneElement = doc.createElement("pane");

        paneElement.setAttribute("id", pane->id());

        panesElement.appendChild(paneElement);
    }

    parentNode->appendChild(domElement);
}

void DockingPaneTabbedContainer::setVisiblePane(DockingPaneContainer *pane)
{
    if (m_paneList.contains(pane)) {
        m_stackedWidget->setCurrentIndex(m_paneList.indexOf(pane));

        setName(m_paneList.at(m_paneList.indexOf(pane))->name());

        update();

        m_stackedWidget->currentWidget()->setFocus();
    }
}

bool DockingPaneTabbedContainer::containsPane(DockingPaneContainer *pane)
{
    return(m_paneList.contains(pane));
}
