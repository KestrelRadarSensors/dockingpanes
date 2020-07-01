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

#ifndef DOCKINGPANECONTAINER_H
#define DOCKINGPANECONTAINER_H

#include <QWidget>
#include <QGridLayout>
#include "dockingpanebase.h"
#include "DockingPaneTitleWidget.h"
#include "DockingToolButton.h"
#include "DockingPaneGlow.h"

class DockingPaneManager;
class DockingPaneFlyoutWidget;

class DockingPaneContainer : public DockingPaneBase
{
    Q_OBJECT

    public:
        enum FlyoutPosition
        {
            Left,
            Right,
            Top,
            Bottom
        };

    public:
        explicit DockingPaneContainer(QString title, QString id, QWidget *parent = 0, QWidget *clientWidget = 0);
        explicit DockingPaneContainer(QWidget *parent = 0);
        void floatPane(QRect rect);
        void floatPane(QPoint pos);
        virtual int getPaneCount(void);
        virtual DockingPaneContainer *getPane(int index);
        virtual DockingPaneFlyoutWidget *openFlyout(bool hasFocus, QWidget *parent, FlyoutPosition pos, DockingPaneContainer *pane);
        friend class DockingPaneManager;

        QWidget *clientWidget();
        virtual void setClientWidget(QWidget *widget);

        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false);

        QSize flyoutSize(void);
        void setFlyoutSize(QSize flyoutSize);
        DockingPaneGlow *floatingGlow(void);
        void setState(DockingPaneBase::State state);

    protected:
        virtual void setName(QString name);
        void setActivePane(bool active);
        virtual void paintEvent(QPaintEvent* event);

    public slots:
        void onCloseButtonClicked(void);
        void onPinButtonClicked(void);
        void onFClicked(void);

    protected slots:
        void onStartDragTitle(QPoint pos);
        void onEndDragTitle(QPoint pos);
        void onMoveDragTitle(QPoint pos);

        void onStartDragFlyoutTitle(QPoint pos);
        void onEndDragFlyoutTitle(QPoint pos);
        void onMoveDragFlyoutTitle(QPoint pos);

    private slots:
        void onUnpinContainer(void);
        void onFocusChanged(QWidget *old, QWidget *now);
        void onCloseContainer(void);

    protected:
        QWidget *m_headerWidget;
        QWidget *m_clientWidget;
        QGridLayout *m_clientLayout;

        DockingToolButton *m_closeButton;
        DockingToolButton *m_pinButton;

        bool m_isActive;
        QPoint m_initialPos;

        DockingPaneTitleWidget *m_titleWidget;
        DockingPaneFlyoutWidget *m_flyoutWidget;

        QSize m_flyoutSize;

        DockingPaneGlow *m_floatingGlow;

        bool m_draggingFlyout;     
};

#endif // DOCKINGPANECONTAINER_H
