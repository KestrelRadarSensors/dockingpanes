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

#ifndef DOCKINGPANEFLYOUTWIDGET_H
#define DOCKINGPANEFLYOUTWIDGET_H

#include <QWidget>

class QGridLayout;

class DockingPaneContainer;
class DockingToolButton;
class DockingPaneTitleWidget;

class DockingPaneFlyoutWidget : public QWidget
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
        explicit DockingPaneFlyoutWidget(bool hasFocus, QRect clientRect, DockingPaneContainer *container, DockingPaneContainer *pane, FlyoutPosition pos, QWidget *widget, QWidget *parent = 0);
        ~DockingPaneFlyoutWidget();

        void restorePaneWidget();
        DockingPaneContainer *pane(void);
        void beginDrag(void);
        void endDrag(void);
        QWidget *clientWidget(void);
        QRect paneRect(void);


    private:
        void setActivePane(bool active);
        void setPositionAndSize(void);
        void updateCursor(void);

    protected:
        virtual void paintEvent(QPaintEvent* event);
        virtual void closeEvent(QCloseEvent *event);
        void resizeEvent(QResizeEvent * event);

        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual bool eventFilter(QObject *obj, QEvent *event);

        virtual void enterEvent(QEvent *e);
        virtual void leaveEvent(QEvent *e);

    signals:
        void unpinContainer(void);
        void closeContainer(void);
        void startDragFlyoutTitle(QPoint pos);
        void endDragFlyoutTitle(QPoint pos);
        void moveDragFlyoutTitle(QPoint pos);
        void flyoutFocusLost(void);
        void autoHideFlyout(void);

    private slots:
        void onFocusChanged(QWidget *old,QWidget *now);
        void autoHideTimeout(void);

    public slots:

    private:
        DockingPaneContainer *m_pane;
        DockingPaneContainer *m_container;
        QWidget *m_clientWidget;
        DockingPaneTitleWidget *m_titleWidget;
        QGridLayout *m_clientLayout;
        bool m_isActive;
        QWidget *m_headerWidget;
        DockingToolButton *m_closeButton;
        DockingToolButton *m_pinButton;
        QPoint m_initialPos;
        FlyoutPosition m_pos;
        int m_size;
        bool m_dragMode;
        bool m_resizeMode;
};

#endif // DOCKINGPANEFLYOUTWIDGET_H
