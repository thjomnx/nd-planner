/*
 * This file is part of 'telamon'.
 *
 *    'telamon' is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    'telamon' is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with 'telamon'. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow()
    : QMainWindow()
{
    setupUi(this);

    createChilds();
    createActions();
    createMenus();

    makeConnections();
}

void MainWindow::createChilds()
{
    mapPanel = new MapPanel(this);
    mapPanel->hide();
}

void MainWindow::createActions()
{
    action_Quit = new QAction(tr("&Quit"), this);
    action_Quit->setShortcut(QKeySequence::Quit);
    action_Quit->setStatusTip(tr("Quit"));

    action_MapPanel = new QAction(tr("&Map"), this);
    action_MapPanel->setCheckable(true);
    action_MapPanel->setShortcut(tr("Ctrl+M"));
    action_MapPanel->setStatusTip(tr("Show or hide the map"));
}

void MainWindow::createMenus()
{
    menu_File->addAction(action_Quit);

    menu_View->addAction(action_MapPanel);
}

void MainWindow::makeConnections()
{
    connect(mapPanel, SIGNAL(visibilityChanged(bool)), action_MapPanel, SLOT(setChecked(bool)));

    connect(action_Quit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(action_MapPanel, SIGNAL(triggered(bool)), mapPanel, SLOT(setVisible(bool)));
}
