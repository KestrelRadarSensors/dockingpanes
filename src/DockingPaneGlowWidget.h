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

#ifndef DOCKINGPANEGLOWWIDGET_H
#define DOCKINGPANEGLOWWIDGET_H

#include <QWidget>

class DockingPaneGlowWidget : public QWidget
{
    Q_OBJECT

    public:
        enum Position
        {
            Left,
            Right,
            Top,
            Bottom
        };

        explicit DockingPaneGlowWidget(QWidget* floatingPane, Position pos, QWidget* parent = nullptr);

        void updatePosition(void);

    private:
        void updateCursor();


    protected:
        virtual void paintEvent(QPaintEvent* event);

        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void enterEvent(QEvent *e);
        virtual void leaveEvent(QEvent *e);

    signals:
        void glowResized();
        
    public slots:
        
    private:
        QWidget *m_floatingPane;
        bool m_dragging;
        QPoint m_Pos;
        QRect m_paneGeometry;
        int m_cursorDelta;
        int m_position;
        int m_cornerState;
};

#endif // DOCKINGPANEGLOWWIDGET_H
