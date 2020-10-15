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

#ifndef DOCKINGTOOLBUTTON_H
#define DOCKINGTOOLBUTTON_H

#include <QPushButton>

class DockingToolButton : public QPushButton
{
    Q_OBJECT

    public:
        enum ButtonType
        {
            closeButtonActive,
            closeButtonInactive,
            pinButtonActive,
            pinButtonInactive,
            unpinButtonActive,
            unpinButtonInactive
        };

        explicit DockingToolButton(ButtonType type, QWidget* parent = nullptr);
        virtual ~DockingToolButton() = default;
        void setButton(DockingToolButton::ButtonType type);

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void enterEvent(QEvent* event) override;
        virtual void leaveEvent(QEvent* event) override;

    private:
        ButtonType m_buttonType;
        bool m_highlight;
};

#endif // DOCKINGTOOLBUTTON_H
