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

        explicit DockingPaneFlyoutWidget(bool hasFocus, DockingPaneContainer *container, DockingPaneContainer *pane, FlyoutPosition pos, QWidget *widget, QWidget *parent = nullptr);
        virtual ~DockingPaneFlyoutWidget() = default;

        void restorePaneWidget();
        DockingPaneContainer *pane(void);
        void beginDrag(void);
        void endDrag(void);
        QWidget *clientWidget(void);
        QRect paneRect(void);

    Q_SIGNALS:
        void unpinContainer(void);
        void closeContainer(void);
        void startDragFlyoutTitle(QPoint pos);
        void endDragFlyoutTitle(QPoint pos);
        void moveDragFlyoutTitle(QPoint pos);
        void flyoutFocusLost(void);
        void autoHideFlyout(void);

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void closeEvent(QCloseEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;

        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual bool eventFilter(QObject* obj, QEvent* event) override;

        virtual void enterEvent(QEvent* event) override;
        virtual void leaveEvent(QEvent* event) override;

    private Q_SLOTS:
        void onFocusChanged(QWidget *old,QWidget *now);

    private:
        void setActivePane(bool active);
        void setPositionAndSize(void);
        void updateCursor(void);
        void autoHideTimeout(void);

        DockingPaneContainer *m_pane;
        DockingPaneContainer *m_container;
        QWidget *m_clientWidget;
        DockingPaneTitleWidget *m_titleWidget;
        QGridLayout *m_clientLayout;

        QWidget *m_headerWidget;
        DockingToolButton *m_closeButton;
        DockingToolButton *m_pinButton;
        QPoint m_initialPos;
        FlyoutPosition m_pos;
        int m_size;
        bool m_isActive;
        bool m_dragMode;
        bool m_resizeMode;
};

#endif // DOCKINGPANEFLYOUTWIDGET_H
