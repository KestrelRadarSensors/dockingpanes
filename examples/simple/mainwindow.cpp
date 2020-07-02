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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DockingPaneManager.h"
#include "DockingPaneBase.h"
#include <QLabel>
#include <QTextEdit>
#include <QUuid>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    showMaximized();

    m_dockingPaneManager = new DockingPaneManager;

    auto clientWidget = new QTextEdit;

    m_dockingPaneManager->setClientWidget(clientWidget);

    setCentralWidget(m_dockingPaneManager->widget());

    DockingPaneBase *dockingWindow_1 = m_dockingPaneManager->createPane(QUuid::createUuid().toString(), "Tool Window 1", createLabel("Hello World 1"), QSize(200, 200), DockingPaneManager::dockBottom);
    DockingPaneBase *dockingWindow_2 = m_dockingPaneManager->createPane(QUuid::createUuid().toString(), "Tool Window 2", createLabel("Hello World 2"), QSize(200, 200), DockingPaneManager::dockLeft, nullptr);
    DockingPaneBase *dockingWindow_3 = m_dockingPaneManager->createPane(QUuid::createUuid().toString(), "Tool Window 3", createLabel("Hello World 3"), QSize(100, 200), DockingPaneManager::dockRight, nullptr);
    DockingPaneBase *dockingWindow_4 = m_dockingPaneManager->createPane(QUuid::createUuid().toString(), "Tool Window 4", createLabel("Hello World 4"), QSize(200, 200), DockingPaneManager::dockBottom, dockingWindow_2);
    DockingPaneBase *dockingWindow_5 = m_dockingPaneManager->createPane(QUuid::createUuid().toString(), "Tool Window 5", createLabel("Hello World 5"), QSize(200, 200), DockingPaneManager::dockFloat, nullptr);

    m_dockingPaneManager->hidePane(dockingWindow_3);

    Q_UNUSED(dockingWindow_1);
    Q_UNUSED(dockingWindow_2);
    Q_UNUSED(dockingWindow_4);
    Q_UNUSED(dockingWindow_5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QLabel *MainWindow::createLabel(QString string)
{
    auto label = new QLabel(string);

    label->setAlignment(Qt::AlignCenter);

    return(label);
}
