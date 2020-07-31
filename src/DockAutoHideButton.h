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

#ifndef DOCKAUTOHIDEBUTTON_H
#define DOCKAUTOHIDEBUTTON_H

#include <QPushButton>

#include "DockingPaneBase.h"
#include "DockingPaneContainer.h"

class QStyleOptionButton;
class QTimer;

class DockAutoHideButton : public QPushButton
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

        DockAutoHideButton(Position pos, QWidget* parent = 0);
        DockAutoHideButton(const QString& text, QWidget* parent = 0);
        DockAutoHideButton(const QIcon& icon, const QString& text, QWidget* parent = 0);

        Qt::Orientation orientation() const;
        void setOrientation(Qt::Orientation orientation);

        bool mirrored() const;
        void setMirrored(bool mirrored);

        void swapDirection(bool state);

        virtual QSize sizeHint() const override;
        Position position(void);
        void setPane(DockingPaneContainer *container, DockingPaneBase *pane);

        DockingPaneBase *pane(void);
        DockingPaneContainer *container(void);

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void enterEvent(QEvent *event) override;
        virtual void leaveEvent(QEvent *event) override;

    Q_SIGNALS:
        void openFlyout(void);

    private:
        QStyleOptionButton* getStyleOption() const;
        void init();
        void onTimerElapsed(void);

        Qt::Orientation m_orientation;
        bool m_mirrored;
        bool m_hovered;
        DockingPaneBase *m_dockingPane;
        DockingPaneContainer *m_paneContainer;

        bool m_swapDirection;
        Position m_pos;
        QTimer *m_hoverTimer;
};

#endif // DOCKAUTOHIDEBUTTON_H
