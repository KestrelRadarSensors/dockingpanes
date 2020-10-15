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

#ifndef DOCKINGFRAMECENTRESTICKER_H
#define DOCKINGFRAMECENTRESTICKER_H

#include <QMap>
#include <QWidget>

class QPaintEvent;

class DockingFrameFrameSticker;

class DockingFrameStickers : public QWidget
{
    Q_OBJECT

    public:
        enum DockingPosition
        {
            paneLeft,
            paneRight,
            paneTop,
            paneBottom,
            frameLeft,
            frameRight,
            frameTop,
            frameBottom,
            tab
        };

    public:
        explicit DockingFrameStickers(QWidget *parent = nullptr);

        void setFrameRect(QRect rect);
        void updateCursorPos(QPoint pos);
        void setTabVisible(bool state);
        bool getHit(QPoint pos, DockingPosition *dockPos);

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void hideEvent(QHideEvent *e) override;
        virtual void showEvent(QShowEvent *e) override;

    private:
        enum StickerPosition
        {
            Centre,
            Left,
            Right,
            Top,
            Bottom,
            Tab,
        };

        void initializeStickersImages(void);
        bool m_isActive;
        bool m_tabVisible;

        QRect m_rcCentre;
        QRect m_rcLeft;
        QRect m_rcRight;
        QRect m_rcTop;
        QRect m_rcBottom;
        QRect m_rcTab;

        DockingFrameFrameSticker *m_frameLeftSticker;
        DockingFrameFrameSticker *m_frameRightSticker;
        DockingFrameFrameSticker *m_frameTopSticker;
        DockingFrameFrameSticker *m_frameBottomSticker;

        QMap<StickerPosition, QImage> m_activeStickers;
        QMap<StickerPosition, QImage> m_inactiveStickers;

};

#endif // DOCKINGFRAMECENTRESTICKER_H
