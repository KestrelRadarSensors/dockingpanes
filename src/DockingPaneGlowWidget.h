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

    Q_SIGNALS:
        void glowResized();

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void enterEvent(QEvent* event) override;
        virtual void leaveEvent(QEvent* event) override;

    private:
        void updateCursor();

        QWidget* m_floatingPane;
        QRect m_paneGeometry;
        QPoint m_Pos;
        int m_position;
        int m_cursorDelta;
        int m_cornerState;
        bool m_dragging;
};

#endif // DOCKINGPANEGLOWWIDGET_H
