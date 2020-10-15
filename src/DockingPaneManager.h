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

#ifndef DOCKINGPANEMANAGER_H
#define DOCKINGPANEMANAGER_H

#include "DockingPanes_global.h"
#include <QWidget>

class QBoxLayout;
class QDomDocument;
class QDomNode;
class DockingPaneBase;
class DockingPaneSplitterContainer;
class DockingPaneManagerPrivate;
class DockingFrameStickers;
class DockingTargetWidget;
class DockingPaneTitleWidget;
class DockingPaneFlyoutWidget;
class DockAutoHideButton;

class DOCKINGPANESSHARED_EXPORT DockingPaneManager : QObject
{
    Q_OBJECT

    public:
        enum DockPosition
        {
            dockLeft,
            dockRight,
            dockTop,
            dockBottom,
            dockFloat,
            dockTab
        };

    public:
        DockingPaneManager();

        QWidget *widget(void);

        DockingPaneBase *setClientWidget(QWidget *widget);

        DockingPaneBase *createPane(QString id, QString title, QWidget *widget, QSize initialSize, DockingPaneManager::DockPosition dockPosition, DockingPaneBase *neighbourPane=NULL);

        void closePane(QString id);
        void closePane(DockingPaneBase *dockingPane);

        void hidePane(DockingPaneBase *dockingPane);
        void showPane(DockingPaneBase *dockingPane);
        void deletePane(DockingPaneBase *pane);
        void unpinPane(DockingPaneBase *pane);
        void closePinnedPane(DockingPaneBase *pane);
        void openFlyout(DockAutoHideButton *button);
        void replacePane(DockingPaneBase *oldPane, DockingPaneBase *newPane);
        void updateAutohideButton(DockingPaneBase *oldContainer, DockingPaneBase *oldPane, DockingPaneBase *newContainer, DockingPaneBase *newPane);
        DockingPaneBase *dockPane(DockingPaneBase *newPane, DockingPaneManager::DockPosition dockPosition, DockingPaneBase *neighbourPane);

        QString saveLayout(QString id);
        bool applyLayout(QString layout);

        void setMainWindow(QWidget *widget);
        QWidget *mainWindow(void);

        void dumpPaneList(void);

    public:
        void floatingPaneMoved(DockingPaneBase *pane, QPoint cursorPos);
        void floatingPaneEndMove(DockingPaneBase *pane, QPoint cursorPos);
        void floatingPaneStartMove(DockingPaneBase *pane, QPoint cursorPos);
        void removePinnedButton(DockingPaneBase *dockingPaneContainer, DockingPaneBase *dockingPane=NULL);

    private:
        friend class DockingPaneContainer;
        friend class DockingPaneTabbedContainer;

        DockPosition getClientPaneDirection(DockingPaneBase *dockingPane);
        bool containsPane(QWidget *widget, QWidget *child);
        void saveFloatingState(QDomNode *parentNode);

        DockingPaneBase *restoreLayout(QDomNode node);
        void savePinnedState(QDomNode *parentNode, QBoxLayout *layout);
        void restorePinnedPanes(QDomNode *node);
        void reparentPane(DockingPaneSplitterContainer *previousParentSplitter, DockingPaneBase *dockingPane);
        void restoreFloatingPanes(QDomNode *node);

    protected:
        bool eventFilter(QObject *obj, QEvent *event);

    private:
        void setWidget(QWidget *widget);
        DockingPaneBase *getDockingParent(QWidget *widget);

        void updateAllSplitters(DockingPaneSplitterContainer *parentSplitter=NULL, bool *containsClient=NULL);
        void updateFloatingPane(DockingPaneBase *currentPane, QPoint cursorPos);

    private Q_SLOTS:
        void onAutoDockButtonClicked(void);
        void onFocusChanged(QWidget *old, QWidget *now);
        void onFlyoutFocusLost(void);

    protected:
        DockingPaneManagerPrivate *const d_ptr;

    private:

        Q_DECLARE_PRIVATE(DockingPaneManager)
};

#endif // DOCKINGPANEMANAGER_H
