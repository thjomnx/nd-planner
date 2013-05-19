/*
 * This file is part of 'nd-planner'.
 *
 *    'nd-planner' is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    'nd-planner' is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with 'nd-planner'. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"

#include "mapcontrol.h"
#include "osmmapadapter.h"
#include "maplayer.h"
#include "geometrylayer.h"
#include "linestring.h"

class Airac;

using namespace qmapcontrol;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
    void setAiracPath();
    void drawAirports();
    void drawNavaids();
    void drawWaypoints();
    void drawAirways();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    void createChilds();
    void createActions();
    void createMenus();

    void makeConnections();

    void createMap();

    LineString* createAirway(const QList<Point*> &points);

    QAction *action_loadAirac;
    QAction *action_Quit;

    MapControl *m_mapController;
    MapAdapter *m_mapAdapter;
    Layer *m_groundLayer;

    GeometryLayer *m_airportlayer;
    GeometryLayer *m_navaidLayer;
    GeometryLayer *m_waypointLayer;
    GeometryLayer *m_airwayLayer;

    QPixmap *m_airportPixmap;
    QPixmap *m_navaidPixmap;
    QPixmap *m_waypointPixmap;

    QPen *m_legPeg;

    Airac *m_airac;
};

#endif // MAINWINDOW_H
