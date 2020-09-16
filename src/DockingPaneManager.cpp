#include "DockingPaneManager.h"
#include "DockingPaneContainer.h"
#include "DockingPaneSplitterContainer.h"
#include "DockingPaneTabbedContainer.h"
#include <QDebug>
#include "DockAutoHideButton.h"
#include "DockingFrameStickers.h"
#include "DockingTargetWidget.h"
#include "DockingPaneClient.h"
#include "DockingPaneBase.h"
#include "DockingPaneFlyoutWidget.h"
#include <QApplication>
#include <QSplitter>

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

#include <QDomDocument>
#include <QBoxLayout>

class DockingPaneManagerPrivate
{
    public:
        DockingPaneManagerPrivate(DockingPaneManager *parent) :
            q_ptr(parent)
        {
            Q_INIT_RESOURCE(images);
        }

    private:
        DockingPaneManager * const q_ptr;

        Q_DECLARE_PUBLIC(DockingPaneManager)

    private:
        DockingPaneClient *m_clientPane;
        DockingPaneBase *m_rootPane;

        QWidget *m_thisWidget;

        QWidget *m_topAutoHidePane;
        QWidget *m_bottomAutoHidePane;
        QWidget *m_leftAutoHidePane;
        QWidget *m_rightAutoHidePane;

        QGridLayout *m_autoHideLayout;

        DockingTargetWidget *m_targetWidget;
        DockingFrameStickers *m_dockingStickers;

        QWidget *m_dockingWidget;

        QList<DockingPaneBase *> m_dockingPaneList;
        QMap<QString, DockingPaneBase *> m_dockingPaneMap;

        int m_targetPosition;
        DockingPaneBase *m_targetPane;

        QWidget *m_mainWindow;

        DockingPaneFlyoutWidget *m_flyoutWidget;
};

DockingPaneManager::DockingPaneManager() :
    d_ptr(new DockingPaneManagerPrivate(this))
{
    Q_D(DockingPaneManager);

    d->m_dockingWidget = NULL;
    d->m_mainWindow = NULL;
    d->m_flyoutWidget = NULL;

    d->m_clientPane = new DockingPaneClient();

    d->m_rootPane = d->m_clientPane;

    d->m_thisWidget = new QWidget();

    d->m_thisWidget->setLayout(new QGridLayout());

    d->m_thisWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->m_thisWidget->layout()->setMargin(5);
    d->m_thisWidget->layout()->setSpacing(0);

    setWidget(d->m_rootPane);

    d->m_targetWidget = new DockingTargetWidget();
    d->m_dockingStickers = new DockingFrameStickers();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget *, QWidget *)));

}

DockingPaneBase *DockingPaneManager::setClientWidget(QWidget *widget)
{
    Q_D(DockingPaneManager);

    d->m_clientPane->setWidget(widget);

    return(d->m_clientPane);
}

void DockingPaneManager::setWidget(QWidget *widget)
{
    Q_D(DockingPaneManager);

    QLayoutItem *child;

    while((child = d->m_thisWidget->layout()->takeAt(0))!=0);

    if (widget) {
        d->m_thisWidget->layout()->addWidget(widget);
    }
}

DockingPaneBase *DockingPaneManager::getDockingParent(QWidget *widget)
{
    while(widget) {
        if (qobject_cast<DockingPaneSplitterContainer *>(widget)) {
            return(qobject_cast<DockingPaneSplitterContainer *>(widget));
        }

        widget = widget->parentWidget();
    }

    return(NULL);
}

DockingPaneBase *DockingPaneManager::createPane(QString id, QString title, QWidget *widget, QSize initialSize, DockingPaneManager::DockPosition dockPosition, DockingPaneBase *neighbourPane)
{
    Q_D(DockingPaneManager);

    DockingPaneContainer *newPane = new DockingPaneContainer(title, id, NULL, widget);

    newPane->resize(initialSize);

    newPane->m_dockingManager = this;

    d->m_dockingPaneList.append(newPane);

    d->m_dockingPaneMap[id] = newPane;

    if (dockPosition==dockFloat) {
        newPane->floatPane(QRect(QPoint(0,0),initialSize));

        newPane->show();

        return(newPane);
    }

    dockPane(newPane, dockPosition, neighbourPane);

    return(newPane);
}

DockingPaneBase *DockingPaneManager::dockPane(DockingPaneBase *paneToDock, DockPosition dockPosition, DockingPaneBase *neighbourPane)
{
    Q_D(DockingPaneManager);

    DockingPaneSplitterContainer *parentSplitter, *newSplitter;
    DockingPaneSplitterContainer::SplitterDirection direction;

    if (dockPosition==dockTab) {
        DockingPaneContainer *parentContainer = qobject_cast<DockingPaneContainer *>(neighbourPane);

        if (DockingPaneTabbedContainer *tabbedContainer = qobject_cast<DockingPaneTabbedContainer *>(neighbourPane)) {
            // dock in existing tabbed container

            if (tabbedContainer->addPane(qobject_cast<DockingPaneContainer *>(paneToDock))) {
                // we docked a tabbed container into another one, so we need to delete the one we docked

                d->m_dockingPaneList.removeOne(paneToDock);

                paneToDock->deleteLater();

                return(tabbedContainer);
            }

            closePane(paneToDock);
        } else {
            DockingPaneTabbedContainer *newContainer = new DockingPaneTabbedContainer(NULL);

            newContainer->m_dockingManager = this;

            d->m_dockingPaneList.append(newContainer);

            replacePane(neighbourPane, newContainer);

            newContainer->addPane(parentContainer);
            newContainer->addPane(qobject_cast<DockingPaneContainer *>(paneToDock));

            closePane(paneToDock);

            newContainer->setState(DockingPaneBase::Docked);
            paneToDock->setState(DockingPaneBase::Tabbed);
            parentContainer->setState(DockingPaneBase::Tabbed);

            return(newContainer);
        }

        return(paneToDock);
    }

    // *****
    // docking with the frame, so set neighbour to root?
    // *****

    if (neighbourPane==NULL) {
        neighbourPane = d->m_rootPane;

        parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(neighbourPane));
    } else {
        parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(neighbourPane->parentWidget()));
    }

    if ( (dockPosition==dockLeft) || (dockPosition==dockRight)) {
        direction = DockingPaneSplitterContainer::splitHorizontal;
    } else {
        direction = DockingPaneSplitterContainer::splitVertical;
    }

    // *****
    // we need to create a new splitter
    // *****

    newSplitter = new DockingPaneSplitterContainer(NULL, direction);

    int positionIndex = 0;

    if (parentSplitter) {
        positionIndex = parentSplitter->m_splitterWidget->indexOf(neighbourPane);
    }

    if (direction==DockingPaneSplitterContainer::splitVertical) {
        paneToDock->resize(neighbourPane->width(), paneToDock->height());
    } else {
        paneToDock->resize(paneToDock->width(), neighbourPane->height());
    }

    QList<int> sizes, newSplitterSizes;

    if (parentSplitter) {
        sizes = parentSplitter->m_splitterWidget->sizes();
    }

    neighbourPane->setParent(NULL);

    if ( (dockPosition==dockLeft) || (dockPosition==dockTop)) {
        if (direction==DockingPaneSplitterContainer::splitHorizontal) {
            int dockedPaneWidth = paneToDock->width();

            if (dockedPaneWidth>(neighbourPane->width()/2)) {
                dockedPaneWidth =  neighbourPane->width()/2;
            }

            newSplitterSizes.append(dockedPaneWidth);
            newSplitterSizes.append(neighbourPane->width()-dockedPaneWidth-newSplitter->m_splitterWidget->handleWidth());
        } else {
            int dockedPaneHeight = paneToDock->height();

            if (dockedPaneHeight>(neighbourPane->height()/2)) {
                dockedPaneHeight =  neighbourPane->height()/2;
            }

            newSplitterSizes.append(dockedPaneHeight);
            newSplitterSizes.append(neighbourPane->height()-dockedPaneHeight-newSplitter->m_splitterWidget->handleWidth());
        }

        newSplitter->m_splitterWidget->addWidget(paneToDock);
        newSplitter->m_splitterWidget->addWidget(neighbourPane);

        newSplitter->m_splitterWidget->setSizes(newSplitterSizes);
    } else {
        if (direction==DockingPaneSplitterContainer::splitHorizontal) {
            int dockedPaneWidth = paneToDock->width();

            if (dockedPaneWidth>(neighbourPane->width()/2)) {
                dockedPaneWidth =  neighbourPane->width()/2;
            }

            newSplitterSizes.append(neighbourPane->width()-dockedPaneWidth-newSplitter->m_splitterWidget->handleWidth());
            newSplitterSizes.append(dockedPaneWidth);
        } else {
            int dockedPaneHeight = paneToDock->height();

            if (dockedPaneHeight>(neighbourPane->height()/2)) {
                dockedPaneHeight =  neighbourPane->height()/2;
            }

            newSplitterSizes.append(neighbourPane->height()-dockedPaneHeight-newSplitter->m_splitterWidget->handleWidth());
            newSplitterSizes.append(dockedPaneHeight);
        }

        newSplitter->m_splitterWidget->addWidget(neighbourPane);
        newSplitter->m_splitterWidget->addWidget(paneToDock);

        newSplitter->m_splitterWidget->setSizes(newSplitterSizes);
    }

    if (neighbourPane==d->m_rootPane) {
        setWidget(newSplitter);

        d->m_rootPane = newSplitter;
    } else {
        if (parentSplitter) {
            parentSplitter->m_splitterWidget->insertWidget(positionIndex, newSplitter);

            parentSplitter->m_splitterWidget->setSizes(sizes);
        }
    }

    updateAllSplitters();

    paneToDock->setState(DockingPaneBase::Docked);

    return(paneToDock);
}

void DockingPaneManager::deletePane(DockingPaneBase *pane)
{
    Q_D(DockingPaneManager);

    d->m_dockingPaneList.removeOne(pane);

    pane->deleteLater();
}

void DockingPaneManager::updateAutohideButton(DockingPaneBase *oldContainer, DockingPaneBase *oldPane, DockingPaneBase *newContainer, DockingPaneBase *newPane)
{
    Q_D(DockingPaneManager);

    QWidget *widgets[] = {d->m_topAutoHidePane, d->m_bottomAutoHidePane, d->m_leftAutoHidePane, d->m_rightAutoHidePane, NULL};

    for (int i=0;widgets[i];i++) {
        QBoxLayout *layout = (QBoxLayout *) widgets[i]->layout();

        for (int j=0;j<layout->count();j++) {
            DockAutoHideButton *button = qobject_cast<DockAutoHideButton *>(widgets[i]->layout()->itemAt(j)->widget());

            if (button) {
                if ( (button->container()==qobject_cast<DockingPaneContainer *>(oldContainer)) &&
                     (button->pane()==oldPane)) {
                    button->setPane(qobject_cast<DockingPaneContainer *>(newContainer), newPane);
                }
            }
        }
    }
}

void DockingPaneManager::replacePane(DockingPaneBase *oldPane, DockingPaneBase *newPane)
{
    DockingPaneSplitterContainer *parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(oldPane));

    if (parentSplitter) {
        QByteArray state = parentSplitter->m_splitterWidget->saveState();

        if (parentSplitter->direction()==DockingPaneSplitterContainer::splitHorizontal) {
            dockPane(newPane, dockLeft, oldPane);
        } else {
            dockPane(newPane, dockRight, oldPane);
        }

        closePane(oldPane);

        parentSplitter->m_splitterWidget->restoreState(state);

        // then we are in a splitter, otherwise the old pane was floating
    }
}

void DockingPaneManager::updateAllSplitters(DockingPaneSplitterContainer *parentSplitter, bool *containsClient)
{
    Q_D(DockingPaneManager);

    DockingPaneClient *clientContainer;
    DockingPaneSplitterContainer *childSplitter;

    if (!parentSplitter) {
        DockingPaneSplitterContainer *rootSplitter = qobject_cast<DockingPaneSplitterContainer *>(d->m_rootPane);
        bool containsClient;

        containsClient = 0;

        if (rootSplitter) {
            updateAllSplitters(rootSplitter, &containsClient);
        }

        return;
    }

    for (int i=0;i<parentSplitter->m_splitterWidget->count();i++) {
        bool childContainsClient;

        childSplitter = qobject_cast<DockingPaneSplitterContainer *>(parentSplitter->m_splitterWidget->widget(i));
        clientContainer = qobject_cast<DockingPaneClient *>(parentSplitter->m_splitterWidget->widget(i));

        if (childSplitter) {
            childContainsClient = false;

            updateAllSplitters(childSplitter, &childContainsClient);

            if (childContainsClient) {
                parentSplitter->m_splitterWidget->setStretchFactor(i, 1);
            } else {
                parentSplitter->m_splitterWidget->setStretchFactor(i, 0);
            }
        } else {
            if (clientContainer) {
                *containsClient = true;

                parentSplitter->m_splitterWidget->setStretchFactor(i, 1);
            } else {
                parentSplitter->m_splitterWidget->setStretchFactor(i, 0);
            }
        }
    }
}


bool DockingPaneManager::eventFilter(QObject *obj, QEvent *event)
{
    Q_D(DockingPaneManager);
    QList<QObject *> widgetList = QList<QObject *>() << d->m_leftAutoHidePane << d->m_rightAutoHidePane << d->m_topAutoHidePane << d->m_bottomAutoHidePane;

    switch(event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease: {
            if (widgetList.contains(obj)) {
                if (d->m_flyoutWidget) {
                    d->m_flyoutWidget->close();

                    return(true);
                }
            }

            break;
        }

        case QEvent::Resize: {
            if (obj==d->m_leftAutoHidePane) {
                d->m_leftAutoHidePane->setFixedWidth(10+d->m_leftAutoHidePane->fontMetrics().height());
                d->m_leftAutoHidePane->setMaximumWidth(10+d->m_leftAutoHidePane->fontMetrics().height());
            }

            if (obj==d->m_rightAutoHidePane) {
                d->m_rightAutoHidePane->setFixedWidth(10+d->m_leftAutoHidePane->fontMetrics().height());
                d->m_rightAutoHidePane->setMaximumWidth(10+d->m_leftAutoHidePane->fontMetrics().height());
            }

            if (obj==d->m_topAutoHidePane) {
                d->m_topAutoHidePane->setFixedHeight(10+d->m_leftAutoHidePane->fontMetrics().height());
                d->m_topAutoHidePane->setMaximumHeight(10+d->m_leftAutoHidePane->fontMetrics().height());
            }

            if (obj==d->m_bottomAutoHidePane) {
                d->m_bottomAutoHidePane->setFixedHeight(10+d->m_leftAutoHidePane->fontMetrics().height());
                d->m_bottomAutoHidePane->setMaximumHeight(10+d->m_leftAutoHidePane->fontMetrics().height());
            }

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}

QWidget *DockingPaneManager::widget(void)
{
    Q_D(DockingPaneManager);

    if (d->m_dockingWidget) {
        return(d->m_dockingWidget);
    }

    d->m_dockingWidget = new QWidget();

    d->m_topAutoHidePane = new QWidget();
    d->m_bottomAutoHidePane = new QWidget();
    d->m_leftAutoHidePane = new QWidget();
    d->m_rightAutoHidePane = new QWidget();

    d->m_leftAutoHidePane->installEventFilter(this);
    d->m_rightAutoHidePane->installEventFilter(this);
    d->m_topAutoHidePane->installEventFilter(this);
    d->m_bottomAutoHidePane->installEventFilter(this);

    d->m_topAutoHidePane->setLayout(new QHBoxLayout);
    d->m_topAutoHidePane->layout()->setContentsMargins(5,0,5,0);
    d->m_topAutoHidePane->layout()->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    d->m_bottomAutoHidePane->setLayout(new QHBoxLayout);
    d->m_bottomAutoHidePane->layout()->setContentsMargins(5,0,5,0);
    d->m_bottomAutoHidePane->layout()->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    d->m_leftAutoHidePane->setLayout(new QVBoxLayout);
    d->m_leftAutoHidePane->layout()->setContentsMargins(0,5,0,5);
    d->m_leftAutoHidePane->layout()->addItem(new QSpacerItem(1, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    d->m_rightAutoHidePane->setLayout(new QVBoxLayout);
    d->m_rightAutoHidePane->layout()->setContentsMargins(0,5,0,5);
    d->m_rightAutoHidePane->layout()->addItem(new QSpacerItem(1, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    d->m_leftAutoHidePane->hide();
    d->m_rightAutoHidePane->hide();
    d->m_topAutoHidePane->hide();
    d->m_bottomAutoHidePane->hide();

    d->m_autoHideLayout = new QGridLayout();

    d->m_topAutoHidePane->setFixedHeight(25);
    d->m_topAutoHidePane->setMaximumHeight(25);
    d->m_bottomAutoHidePane->setFixedHeight(25);
    d->m_bottomAutoHidePane->setMaximumHeight(25);

    d->m_leftAutoHidePane->setFixedWidth(25);
    d->m_leftAutoHidePane->setMaximumWidth(25);
    d->m_rightAutoHidePane->setFixedWidth(25);
    d->m_rightAutoHidePane->setMaximumWidth(25);

    d->m_leftAutoHidePane->setMinimumWidth(0);
    d->m_rightAutoHidePane->setMinimumWidth(0);

    d->m_autoHideLayout->addWidget(d->m_topAutoHidePane, 0, 1);
    d->m_autoHideLayout->addWidget(d->m_leftAutoHidePane, 1, 0);
    d->m_autoHideLayout->addWidget(d->m_thisWidget, 1, 1);
    d->m_autoHideLayout->addWidget(d->m_rightAutoHidePane, 1, 2);
    d->m_autoHideLayout->addWidget(d->m_bottomAutoHidePane, 2, 1);

    d->m_autoHideLayout->setMargin(0);
    d->m_autoHideLayout->setSpacing(0);

    d->m_dockingWidget->setLayout(d->m_autoHideLayout);

    return(d->m_dockingWidget);
}

void DockingPaneManager::closePane(QString id)
{
    Q_D(DockingPaneManager);

    if (d->m_dockingPaneMap.contains(id)) {
        closePane( d->m_dockingPaneMap[id]);
    }
}

void DockingPaneManager::removePinnedButton(DockingPaneBase *dockingPaneContainer, DockingPaneBase *dockingPane)
{
    Q_D(DockingPaneManager);

    QWidget *widgets[] = {d->m_topAutoHidePane, d->m_bottomAutoHidePane, d->m_leftAutoHidePane, d->m_rightAutoHidePane, NULL};
    QBoxLayout *layout;
    QList<QWidget *> deleteList;

    for (int i=0;widgets[i];i++) {
        layout = (QBoxLayout *) widgets[i]->layout();

        for (int j=0;j<layout->count();j++) {
            DockAutoHideButton *button = qobject_cast<DockAutoHideButton *>(widgets[i]->layout()->itemAt(j)->widget());

            if (button) {
                if ((button->container()==dockingPaneContainer) && ( (dockingPane==NULL) || ((dockingPane) && (button->pane()==dockingPane))) ) {
                    if (!deleteList.contains(dockingPane)) {
                        deleteList.append(button);
                    }
                }
            }
        }
    }

    foreach(QWidget *widget, deleteList) {
        QWidget *parent;

        parent = widget->parentWidget();

        delete widget;

        if (parent->layout()->count()==1) {
            parent->hide();
        }
    }
}

void DockingPaneManager::closePane(DockingPaneBase *dockingPane)
{
    DockingPaneSplitterContainer *parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(dockingPane->parentWidget()));

    if (dockingPane->state()==DockingPaneBase::Pinned) {
        removePinnedButton(dockingPane);
    }

    if (DockingPaneContainer *containerPane=qobject_cast<DockingPaneContainer *>(dockingPane)) {
        if (containerPane->state()==DockingPaneBase::Floating) {
            containerPane->hide();
        }

        containerPane->setState(DockingPaneBase::Hidden);
    }

    if (parentSplitter) {
        dockingPane->setParent(NULL);

        if (parentSplitter->m_splitterWidget->count()==1) {
            // *****
            // single container left in splitter, so needs reparenting
            // *****

            reparentPane(parentSplitter, qobject_cast<DockingPaneBase *>(parentSplitter->m_splitterWidget->widget(0)));
        } else {
            bool parentPaneVisible = false;

            for(int i=0;i<parentSplitter->m_splitterWidget->count();i++) {
                if (!parentSplitter->m_splitterWidget->widget(i)->isHidden()) {
                    parentPaneVisible = true;
                    break;
                }
            }

            if (!parentPaneVisible) {
                hidePane(parentSplitter);
            }
        }
    }
}

void DockingPaneManager::reparentPane(DockingPaneSplitterContainer *previousParentSplitter, DockingPaneBase *dockingPane)
{
    Q_D(DockingPaneManager);

    DockingPaneBase *paneParent = getDockingParent(previousParentSplitter->parentWidget());
    DockingPaneSplitterContainer *parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(paneParent);

    if (!paneParent) {
        setWidget(dockingPane);

        d->m_rootPane = dockingPane;

        delete previousParentSplitter;

        return;
    }

    if (parentSplitter) {
        QByteArray state = parentSplitter->m_splitterWidget->saveState();

        parentSplitter->m_splitterWidget->insertWidget(parentSplitter->m_splitterWidget->indexOf(previousParentSplitter), dockingPane);

        delete previousParentSplitter;

        parentSplitter->m_splitterWidget->restoreState(state);
    }

    updateAllSplitters();
}

void DockingPaneManager::hidePane(DockingPaneBase *dockingPane)
{
    Q_D(DockingPaneManager);

    DockingPaneSplitterContainer *parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(dockingPane->parentWidget()));

    if (parentSplitter) {
        bool parentPaneVisible = false;

        if (DockingPaneContainer *paneContainer = qobject_cast<DockingPaneContainer *>(dockingPane)) {
            DockPosition pos = getClientPaneDirection(paneContainer);

            paneContainer->setFlyoutSize(dockingPane->size());

            for (int i=0;i<paneContainer->getPaneCount();i++) {
                QWidget *autoHideWidget;
                DockAutoHideButton::Position buttonPos;

                switch(pos) {
                    case dockLeft: {
                        autoHideWidget = d->m_rightAutoHidePane;
                        buttonPos = DockAutoHideButton::Right;

                        break;
                    }

                    case dockRight: {
                        autoHideWidget = d->m_leftAutoHidePane;
                        buttonPos = DockAutoHideButton::Left;

                        break;
                    }

                    case dockTop: {
                        autoHideWidget = d->m_bottomAutoHidePane;
                        buttonPos = DockAutoHideButton::Bottom;

                        break;
                    }

                    case dockBottom: {
                        autoHideWidget = d->m_topAutoHidePane;
                        buttonPos = DockAutoHideButton::Top;

                        break;
                    }

                    default: {
                        return;
                    }
                }

                autoHideWidget->show();

                DockAutoHideButton *button = new DockAutoHideButton( buttonPos);

                button->setText(paneContainer->getPane(i)->name());

                button->setPane(paneContainer, paneContainer->getPane(i));

                connect(button, SIGNAL(clicked()), this, SLOT(onAutoDockButtonClicked()));

                if ((pos==dockLeft) || (pos==dockRight)) {
                    QVBoxLayout *layout = (QVBoxLayout *) autoHideWidget->layout();

                    button->setOrientation(Qt::Vertical);
                    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

                    if (pos==dockLeft) {
                        button->swapDirection(true);
                    }

                    layout->insertWidget(layout->count()-1, button);
                } else {
                    QHBoxLayout *layout = (QHBoxLayout *) autoHideWidget->layout();

                    if (pos==dockTop) {
                        button->swapDirection(true);
                    }

                    layout->insertWidget(layout->count()-1, button);
                }
            }
        }

        dockingPane->hide();
        dockingPane->setState(DockingPaneBase::Pinned);

        for(int i=0;i<parentSplitter->m_splitterWidget->count();i++) {
            if (!parentSplitter->m_splitterWidget->widget(i)->isHidden()) {
                parentPaneVisible = true;
                break;
            }
        }

        if (!parentPaneVisible) {
            hidePane(parentSplitter);
        }
    }
}

DockingPaneManager::DockPosition DockingPaneManager::getClientPaneDirection(DockingPaneBase *dockingPane)
{
    Q_D(DockingPaneManager);

    DockingPaneSplitterContainer *parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(d->m_clientPane));

    while(parentSplitter) {
        for (int i=0;i<parentSplitter->m_splitterWidget->count();i++) {
            if (containsPane(parentSplitter->m_splitterWidget->widget(i), dockingPane)) {
                if (parentSplitter->direction() == DockingPaneSplitterContainer::splitVertical) {
                    for (int j=0;j<i;j++) {
                        if (containsPane(parentSplitter->m_splitterWidget->widget(j), d->m_clientPane))
                            return(dockTop);
                    }

                    return(dockBottom);
                } else {
                    for (int j=0;j<i;j++) {
                        if (containsPane(parentSplitter->m_splitterWidget->widget(j), d->m_clientPane)) {
                            return(dockLeft);
                        }
                    }

                    return(dockRight);
                }

                return(dockLeft);
            }
        }

        parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(parentSplitter->parentWidget()));
    }

    return(DockingPaneManager::dockLeft);
}

bool DockingPaneManager::containsPane(QWidget *widget, QWidget *child)
{
    DockingPaneSplitterContainer *splitter = qobject_cast<DockingPaneSplitterContainer *>(widget);

    if (!splitter) {
        if (widget==child) {
            return(true);
        } else {
            return(false);
        }
    }

    for (int i=0;i<splitter->m_splitterWidget->count();i++) {
        if (containsPane(splitter->m_splitterWidget->widget(i), child)) {
            return(true);
        }
    }

    return(false);
}

void DockingPaneManager::unpinPane(DockingPaneBase *pane)
{
    Q_D(DockingPaneManager);

    if (DockingPaneContainer *container = qobject_cast<DockingPaneContainer *>(pane)) {
        DockingPaneSplitterContainer *parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(container->parentWidget()));

        container->setFlyoutSize(QSize());

        container->show();
        container->setState(DockingPaneBase::Docked);

        while(parentSplitter) {
            parentSplitter->show();

            parentSplitter = qobject_cast<DockingPaneSplitterContainer *>(getDockingParent(parentSplitter->parentWidget()));
        }
    }

    removePinnedButton(pane);

    if (d->m_flyoutWidget) {
        d->m_flyoutWidget->blockSignals(true);
        d->m_flyoutWidget->close();
        delete d->m_flyoutWidget;
        d->m_flyoutWidget = NULL;
    }
}

void DockingPaneManager::closePinnedPane(DockingPaneBase *pane)
{
    Q_D(DockingPaneManager);

    if (DockingPaneContainer *container = qobject_cast<DockingPaneContainer *>(pane)) {
        container->setFlyoutSize(QSize());
    }

    closePane(pane);

    if (d->m_flyoutWidget) {
        d->m_flyoutWidget->blockSignals(true);
        d->m_flyoutWidget->close();
        delete d->m_flyoutWidget;
        d->m_flyoutWidget = NULL;
    }
}

void DockingPaneManager::openFlyout(DockAutoHideButton *button)
{
    Q_D(DockingPaneManager);

    if (d->m_flyoutWidget) {
        if (button->pane()==d->m_flyoutWidget->pane()) {
            return;
        }

        d->m_flyoutWidget->blockSignals(true);

        d->m_flyoutWidget->restorePaneWidget();
        d->m_flyoutWidget->close();

        delete d->m_flyoutWidget;

        d->m_flyoutWidget = NULL;
    }

    d->m_flyoutWidget = button->container()->openFlyout(true, d->m_thisWidget, (DockingPaneContainer::FlyoutPosition) button->position(), qobject_cast<DockingPaneContainer *>(button->pane()));

    connect(d->m_flyoutWidget, SIGNAL(flyoutFocusLost()), this, SLOT(onFlyoutFocusLost()));
}

void DockingPaneManager::onAutoDockButtonClicked(void)
{
    openFlyout((DockAutoHideButton *) sender());
}

void DockingPaneManager::onFlyoutFocusLost(void)
{
    Q_D(DockingPaneManager);

    if (d->m_flyoutWidget) {
        d->m_flyoutWidget->restorePaneWidget();
        d->m_flyoutWidget->deleteLater();
        d->m_flyoutWidget = NULL;
    }
}

void DockingPaneManager::showPane(DockingPaneBase *dockingPane)
{
    Q_D(DockingPaneManager);

    switch(dockingPane->state()) {
        case DockingPaneBase::Pinned:
        {
            QWidget *widgets[] = {d->m_topAutoHidePane, d->m_bottomAutoHidePane, d->m_leftAutoHidePane, d->m_rightAutoHidePane, NULL};

            for (int i=0;widgets[i];i++) {
                QBoxLayout *layout = (QBoxLayout *) widgets[i]->layout();

                for (int j=0;j<layout->count();j++) {
                    DockAutoHideButton *button = qobject_cast<DockAutoHideButton *>(widgets[i]->layout()->itemAt(j)->widget());

                    if (button) {
                        if (button->pane()==dockingPane) {
                            openFlyout(button);

                            return;
                        }
                    }
                }
            }

            break;
        }

        case DockingPaneBase::Hidden: {
            DockingPaneContainer *container = qobject_cast<DockingPaneContainer *>(dockingPane);

            if (container) {
                container->floatPane(QPoint(0,0));
            }

            break;
        }

        case DockingPaneBase::Floating:
        case DockingPaneBase::Docked: {
            qApp->setActiveWindow(dockingPane);

            dockingPane->setFocus();

            break;
        }

        case DockingPaneBase::Tabbed: {
            QWidget *widgets[] = {d->m_topAutoHidePane, d->m_bottomAutoHidePane, d->m_leftAutoHidePane, d->m_rightAutoHidePane, NULL};

            for (int i=0;widgets[i];i++) {
                QBoxLayout *layout = (QBoxLayout *) widgets[i]->layout();

                for (int j=0;j<layout->count();j++) {
                    DockAutoHideButton *button = qobject_cast<DockAutoHideButton *>(widgets[i]->layout()->itemAt(j)->widget());

                    if (button) {
                        if (button->pane()==dockingPane) {
                            openFlyout(button);

                            return;
                        }
                    }
                }
            }

            foreach(DockingPaneBase *pane, d->m_dockingPaneList) {
                DockingPaneTabbedContainer *container = qobject_cast<DockingPaneTabbedContainer *>(pane);

                if (container) {
                    DockingPaneContainer *paneContainer = qobject_cast<DockingPaneContainer *>(dockingPane);

                    if (paneContainer) {
                        if (container->containsPane(paneContainer)) {
                            container->setVisiblePane(paneContainer);

                            return;
                        }
                    }
                }
            }

            break;
        }

        default: {
            break;
        }
    }
}

void DockingPaneManager::onFocusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old);
    Q_UNUSED(now);
}

void DockingPaneManager::floatingPaneStartMove(DockingPaneBase *pane, QPoint cursorPos)
{
    Q_UNUSED(pane);
    Q_UNUSED(cursorPos);
    Q_D(DockingPaneManager);

    QRect rcFrame(d->m_thisWidget->mapToGlobal(d->m_thisWidget->rect().topLeft()), d->m_thisWidget->mapToGlobal(d->m_thisWidget->rect().bottomRight()));

    d->m_dockingStickers->setFrameRect(rcFrame);

    d->m_targetPosition = -1;
    d->m_targetPane = NULL;

    d->m_dockingStickers->setTabVisible(true);
}

void DockingPaneManager::floatingPaneEndMove(DockingPaneBase *pane, QPoint cursorPos)
{
    Q_UNUSED(cursorPos);
    Q_D(DockingPaneManager);

    if (d->m_targetWidget) {
        d->m_targetWidget->hide();
    }

    if (d->m_dockingStickers) {
        d->m_dockingStickers->hide();
    }

    if (d->m_targetPane) {
        switch(d->m_targetPosition) {
            case DockingFrameStickers::paneLeft: {
                d->m_targetPosition = dockLeft;

                break;
            }

            case DockingFrameStickers::paneRight: {
                d->m_targetPosition = dockRight;

                break;
            }

            case DockingFrameStickers::paneTop: {
                d->m_targetPosition = dockTop;

                break;
            }

            case DockingFrameStickers::paneBottom: {
                d->m_targetPosition = dockBottom;

                break;
            }

            case DockingFrameStickers::frameLeft: {
                d->m_targetPosition = dockLeft;
                d->m_targetPane = NULL;

                break;
            }

            case DockingFrameStickers::frameRight: {
                d->m_targetPosition = dockRight;
                d->m_targetPane = NULL;

                break;
            }

            case DockingFrameStickers::frameTop: {
                d->m_targetPosition = dockTop;
                d->m_targetPane = NULL;

                break;
            }

            case DockingFrameStickers::frameBottom: {
                d->m_targetPosition = dockBottom;
                d->m_targetPane = NULL;

                break;
            }

            case DockingFrameStickers::tab: {
                d->m_targetPosition = dockTab;

                break;
            }
        }

        DockingPaneBase *dockedPane = dockPane(pane, (DockPosition) d->m_targetPosition, d->m_targetPane);

        qApp->setActiveWindow(dockedPane);

        dockedPane->setFocus();
    }
}

void DockingPaneManager::floatingPaneMoved(DockingPaneBase *pane, QPoint cursorPos)
{
    Q_D(DockingPaneManager);

    if (d->m_dockingStickers) {
        d->m_dockingStickers->updateCursorPos(cursorPos);
    }

    d->m_targetPosition = -1;
    d->m_targetPane = NULL;

    updateFloatingPane(d->m_clientPane, cursorPos);

    foreach(DockingPaneBase *searchPane, d->m_dockingPaneList) {
        DockingPaneContainer *currentPane = qobject_cast<DockingPaneContainer *>(searchPane);

        if ( (currentPane!=pane) && (currentPane->state()==DockingPaneBase::Docked) ) {
            updateFloatingPane(currentPane, cursorPos);
        }
    }
}

void DockingPaneManager::updateFloatingPane(DockingPaneBase *currentPane, QPoint cursorPos)
{
    Q_D(DockingPaneManager);

    static QRect lastHitRect, lastStickerRect;
    QRect paneRect;

    if (d->m_thisWidget->rect().contains(d->m_thisWidget->mapFromGlobal(cursorPos))) {
        paneRect.setTopLeft(currentPane->mapToGlobal(QPoint(0,0)));
        paneRect.setBottomRight(currentPane->mapToGlobal(QPoint(currentPane->width(), currentPane->height())));

        if (paneRect.contains(cursorPos)) {
            DockingFrameStickers::DockingPosition dockPos;

            if (d->m_dockingStickers) {
                QPoint pt(paneRect.center().x()-(d->m_dockingStickers->width()/2.0), paneRect.center().y()-(d->m_dockingStickers->height()/2.0));

                if (lastStickerRect!=paneRect) {
                    d->m_dockingStickers->hide();
                }

                if (currentPane==d->m_clientPane) {
                    d->m_dockingStickers->setTabVisible(false);
                } else {
                    d->m_dockingStickers->setTabVisible(true);
                }

                d->m_dockingStickers->move(pt);
                d->m_dockingStickers->show();

                lastStickerRect = paneRect;
            }

            if (d->m_dockingStickers->getHit(cursorPos, &dockPos)) {
                d->m_targetPosition = dockPos;

                switch((DockingFrameStickers::DockingPosition) dockPos) {
                    case DockingFrameStickers::paneLeft: {
                        paneRect.setRight(paneRect.left()+(paneRect.width()*0.25));

                        break;
                    }

                    case DockingFrameStickers::paneRight: {
                        paneRect.setLeft(paneRect.right()-(paneRect.width()*0.25));

                        break;
                    }

                    case DockingFrameStickers::paneTop: {
                        paneRect.setBottom(paneRect.top()+(paneRect.height()*0.25));

                        break;
                    }

                    case DockingFrameStickers::paneBottom: {
                        paneRect.setTop(paneRect.bottom()-(paneRect.height()*0.25));

                        break;
                    }

                    default: {
                        break;
                    }
                }

                d->m_targetPane = currentPane;

                if (d->m_targetWidget) {
                    if (lastHitRect!=paneRect) {
                        d->m_targetWidget->hide();
                    }

                    d->m_targetWidget->move(paneRect.topLeft());
                    d->m_targetWidget->resize(paneRect.size());
                    d->m_targetWidget->show();

                    lastHitRect = paneRect;
                }
            } else {
                if (d->m_targetWidget) {
                    d->m_targetWidget->hide();
                }
            }
        }
    } else {
        d->m_targetWidget->hide();
        d->m_dockingStickers->hide();
    }
}

QString DockingPaneManager::saveLayout(QString layoutId)
{
    Q_D(DockingPaneManager);

    QDomDocument doc;

    QDomElement xmlRootNode = doc.createElement("dockingLayout");

    xmlRootNode.setAttribute("id", layoutId);

    doc.appendChild(xmlRootNode);

    QDomNode nodeToSave = xmlRootNode.appendChild(doc.createElement("dockedLayout"));

    d->m_rootPane->saveLayout(&nodeToSave);

    QDomNode pinNode = xmlRootNode.appendChild(doc.createElement("pinnedPanes"));

    savePinnedState(&pinNode, qobject_cast<QBoxLayout *>(d->m_leftAutoHidePane->layout()));
    savePinnedState(&pinNode, qobject_cast<QBoxLayout *>(d->m_rightAutoHidePane->layout()));
    savePinnedState(&pinNode, qobject_cast<QBoxLayout *>(d->m_topAutoHidePane->layout()));
    savePinnedState(&pinNode, qobject_cast<QBoxLayout *>(d->m_bottomAutoHidePane->layout()));

    QDomNode floatingNode = xmlRootNode.appendChild(doc.createElement("floatingPanes"));

    saveFloatingState(&floatingNode);

    return(doc.toString());
}

bool DockingPaneManager::applyLayout(QString layout)
{
    Q_D(DockingPaneManager);

    QDomDocument doc;
    QString errorMessage;
    bool returnValue = true;

    // ****
    // now apply the layout
    // ****

    doc.setContent(layout, &errorMessage);

    QDomElement docElement = doc.documentElement();

    if (docElement.nodeName()!="dockingLayout") {
        return(false);
    }

    // *****
    // close all the panes!
    // *****

    d->m_thisWidget->setUpdatesEnabled(false);

    foreach(DockingPaneBase *pane, d->m_dockingPaneList) {
        closePane(pane);
    }

    d->m_rootPane = restoreLayout(docElement.firstChildElement("dockedLayout").firstChild());

    // *****
    // check if the restored layout is valid
    // *****

    if (d->m_rootPane) {
        // *****
        // check that the restored layout contains the client, if not, something is very wrong....
        // *****

        if (!containsPane(d->m_rootPane, d->m_clientPane)) {
            d->m_rootPane = d->m_clientPane;

            returnValue = false;
        } else {
            QDomNode pinnedPanes = docElement.firstChildElement("pinnedPanes");
            QDomNode floatingPanes = docElement.firstChildElement("floatingPanes");

            restorePinnedPanes(&pinnedPanes);
            restoreFloatingPanes(&floatingPanes);
        }
    } else {
        d->m_rootPane = d->m_clientPane;
        returnValue = false;
    }

    setWidget(d->m_rootPane);

    updateAllSplitters();

    d->m_thisWidget->setUpdatesEnabled(true);

    return(returnValue);
}

DockingPaneBase *DockingPaneManager::restoreLayout(QDomNode node)
{
    Q_D(DockingPaneManager);

    if (node.nodeName()=="DockingPaneClient") {
        return(d->m_clientPane);
    }

    if (node.nodeName()=="DockingPaneContainer") {
        QDomElement nodeElement = node.toElement();
        QString containerId;

        containerId = nodeElement.attribute("id");

        if (d->m_dockingPaneMap.contains(containerId)) {
            return(d->m_dockingPaneMap[containerId]);
        }

        return(NULL);
    }

    if (node.nodeName()=="DockingPaneTabbedContainer") {
        QDomElement nodeElement = node.toElement();
        QString containerId, selectedTab;
        DockingPaneContainer *selectedContainer = NULL;
        int paneCount = 0;

        DockingPaneTabbedContainer *tabbedContainer = new DockingPaneTabbedContainer();

        tabbedContainer->setId(nodeElement.attribute("id"));

        selectedTab = nodeElement.attribute("selectedTab");

        tabbedContainer->m_dockingManager = this;

        d->m_dockingPaneList.append(tabbedContainer);

        QDomNodeList nodeList = nodeElement.elementsByTagName("pane");

        for(int i=0;i<nodeList.count();i++) {
            containerId = nodeList.at(i).toElement().attribute("id");

            if (d->m_dockingPaneMap.contains(containerId)) {
                DockingPaneContainer *container = qobject_cast<DockingPaneContainer *>(d->m_dockingPaneMap[containerId]);

                if (container) {
                    if (containerId==selectedTab) {
                        selectedContainer = container;
                    }

                    tabbedContainer->addPane(container);
                    container->setState(DockingPaneBase::Tabbed);

                    paneCount++;
                }
            }
        }

        if (paneCount) {
            d->m_dockingPaneMap[nodeElement.attribute("id")] = tabbedContainer;

            if (selectedContainer) {
                tabbedContainer->setVisiblePane(selectedContainer);
            }

            return(tabbedContainer);
        }

        delete tabbedContainer;

        return(NULL);
    }

    if (node.nodeName()=="DockingPaneSplitterContainer")
    {
        QDomElement nodeElement = node.toElement();
        DockingPaneSplitterContainer *newSplitter;

        DockingPaneBase *first = restoreLayout(nodeElement.childNodes().at(0));
        DockingPaneBase *second = restoreLayout(nodeElement.childNodes().at(1));;

        if (first && second) {
            newSplitter = new DockingPaneSplitterContainer;

            newSplitter->m_splitterWidget->addWidget(first);
            newSplitter->m_splitterWidget->addWidget(second);

            first->setVisible(true);
            second->setVisible(true);

            first->setState(DockingPaneBase::Docked);
            second->setState(DockingPaneBase::Docked);

            // *****
            // this is not a bug!  First restore restores the splitter state, second restore restores
            // the child state....weird, but necessary to ensure child windows end up correct size!
            // *****

            newSplitter->m_splitterWidget->restoreState(QByteArray::fromBase64(nodeElement.attribute("state").toLatin1()));
            newSplitter->m_splitterWidget->restoreState(QByteArray::fromBase64(nodeElement.attribute("state").toLatin1()));

            return(newSplitter);
        } else {
            if (first) {
                return(first);
            }

            if (second) {
                return(second);
            }
        }
    }

    return(NULL);
}

void DockingPaneManager::saveFloatingState(QDomNode *parentNode)
{
    Q_D(DockingPaneManager);

    foreach(DockingPaneBase *basePane, d->m_dockingPaneList) {
        if (basePane->state()==DockingPaneBase::Floating) {
            basePane->saveLayout(parentNode, true);
        }
    }
}

void DockingPaneManager::savePinnedState(QDomNode *parentNode, QBoxLayout *layout)
{
    QDomDocument doc = parentNode->ownerDocument();
    QList<QString> savedIds;

    for(int i=0;i<layout->count();i++) {
        DockAutoHideButton *autoHideButton = qobject_cast<DockAutoHideButton *>(layout->itemAt(i)->widget());

        if (autoHideButton) {
            QDomDocument doc = parentNode->ownerDocument();

            if (!savedIds.contains(autoHideButton->container()->id())) {
                QDomElement domElement = doc.createElement("pane");

                domElement.setAttribute("id", autoHideButton->container()->id());

                parentNode->appendChild(domElement);

                savedIds.append(autoHideButton->container()->id());
            }
        }
    }
}

void DockingPaneManager::restorePinnedPanes(QDomNode *node)
{
    Q_D(DockingPaneManager);

    QDomNodeList nodeList = node->childNodes();

    for(int i=0;i<nodeList.count();i++) {
        QDomElement domElement = nodeList.at(i).toElement();

        if (domElement.nodeName()=="pane") {
            QString paneId = domElement.attribute("id");

            if (d->m_dockingPaneMap.contains(paneId)) {
                hidePane(d->m_dockingPaneMap[paneId]);
            }
        }
    }
}

void DockingPaneManager::restoreFloatingPanes(QDomNode *node)
{
    Q_D(DockingPaneManager);

    QDomNodeList nodeList = node->childNodes();

    for(int i=0;i<nodeList.count();i++) {
        QDomElement domElement = nodeList.at(i).toElement();

        QString paneId = domElement.attribute("id");

        if (d->m_dockingPaneMap.contains(paneId)) {
            DockingPaneContainer *container = qobject_cast<DockingPaneContainer *>(d->m_dockingPaneMap[paneId]);

            if (container) {
                container->restoreGeometry(QByteArray::fromBase64(domElement.attribute("geometry").toLatin1()));
                container->floatPane(QPoint(0,0));
            }
        }
    }
}

void DockingPaneManager::setMainWindow(QWidget *window)
{
    Q_D(DockingPaneManager);

    d->m_mainWindow = window;
}

QWidget *DockingPaneManager::mainWindow(void)
{
    Q_D(DockingPaneManager);

    return(d->m_mainWindow);
}

void DockingPaneManager::dumpPaneList(void)
{
    /*Q_D(DockingPaneManager);

    foreach(DockingPaneBase *searchPane, d->m_dockingPaneList) {
        DockingPaneContainer *currentPane = qobject_cast<DockingPaneContainer *>(searchPane);

        if (currentPane) {
            qDebug() << "pane" << currentPane << currentPane->state();
        }
    }*/
}
