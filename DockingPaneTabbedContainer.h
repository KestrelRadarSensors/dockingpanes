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

#ifndef DOCKINGPANETABBEDCONTAINER_H
#define DOCKINGPANETABBEDCONTAINER_H

#include <QWidget>
#include "DockingPaneContainer.h"
#include <QStackedWidget>
#include <QList>
#include "DockingPaneFlyoutWidget.h"

class QMouseEvent;
class DockingPaneManager;

class DockingPaneTabbedContainer : public DockingPaneContainer
{
        Q_OBJECT

    public:
        explicit DockingPaneTabbedContainer(QWidget *parent = 0);
        ~DockingPaneTabbedContainer();

        bool addPane(DockingPaneContainer *child);

        friend class DockingPaneManager;

        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false);

        void restoreChildWidgets(void);
        virtual int getPaneCount(void);

        virtual DockingPaneContainer *getPane(int index);
        virtual DockingPaneFlyoutWidget *openFlyout(bool hasFocus, QWidget *parent, FlyoutPosition pos, DockingPaneContainer *pane);
        virtual void setClientWidget(QWidget *widget);
        void setVisiblePane(DockingPaneContainer *pane);
        bool containsPane(DockingPaneContainer *pane);

    private:
        void calculateButtonsRectangles(void);
        QRect getButtonRect(int pos);
        void updateMargins(void);

    protected:
        virtual void paintEvent(QPaintEvent* event);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void resizeEvent(QResizeEvent * event);

    signals:
        
    protected slots:
        void onStartDragTitle(QPoint pos);
        void onEndDragTitle(QPoint pos);
        void onMoveDragTitle(QPoint pos);

        void onStartDragFlyoutTitle(QPoint pos);
        void onEndDragFlyoutTitle(QPoint pos);
        void onMoveDragFlyoutTitle(QPoint pos);

    private slots:
        void onUnpinContainer(void);
        void onCloseContainer(void);
        void onFocusChanged(QWidget *old, QWidget *now);

    public slots:
        void onCloseButtonClicked(void);
        void onPinButtonClicked(void);

    protected:
        QStackedWidget *m_stackedWidget;
        QList<DockingPaneContainer *> m_paneList;
        QList<int> m_tabWidths;
        DockingPaneContainer *m_draggedPane;
        QPoint m_initialPos;
        QPoint m_originalClickPos;
        QRect m_invalidTabRect;
        DockingPaneFlyoutWidget *m_flyoutWidget;
        bool m_fromMousePressEvent;  
};

#endif // DOCKINGPANETABBEDCONTAINER_H
