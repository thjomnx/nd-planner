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

#include <QtWidgets>

#include "mainwindow.h"

#include "airac.h"
#include "airport.h"
#include "navaid.h"
#include "waypoint.h"
#include "airway.h"
#include "leg.h"

#include "point.h"
#include "imagepoint.h"

MainWindow::MainWindow()
    : QMainWindow()
{
    setupUi(this);

    createChilds();
    createActions();
    createMenus();

    makeConnections();

    createMap();
}

void MainWindow::setAiracPath()
{
    m_airac->loadAirac("/home/menax/Projekte/local/nd-planner/prototyping/RAMZZESS_B777_NATIVE_1304v2/X-Plane/Ramzzess B777");

//     drawAirports();
//     drawNavaids();
//     drawWaypoints();
    drawAirways();
}

void MainWindow::drawAirports()
{
    m_airportlayer = new GeometryLayer("Airports", m_mapAdapter);
    m_mapController->addLayer(m_airportlayer);

    foreach (Airport *ap, m_airac->airports())
    {
        Point *pt = new ImagePoint(ap->longitude(), ap->latitude(), m_airportPixmap, ap->identifier());
        m_airportlayer->addGeometry(pt);
    }
}

void MainWindow::drawNavaids()
{
    m_navaidLayer = new GeometryLayer("Navaids", m_mapAdapter);
    m_mapController->addLayer(m_navaidLayer);

    foreach (Navaid *nav, m_airac->navaids())
    {
        Point *pt = new ImagePoint(nav->longitude(), nav->latitude(), m_navaidPixmap, nav->identifier());
        m_navaidLayer->addGeometry(pt);
    }
}

void MainWindow::drawWaypoints()
{
    m_waypointLayer = new GeometryLayer("Waypoints", m_mapAdapter);
    m_mapController->addLayer(m_waypointLayer);

    foreach (Waypoint *wp, m_airac->waypoints())
    {
        Point *pt = new ImagePoint(wp->longitude(), wp->latitude(), m_waypointPixmap, wp->identifier());
        m_waypointLayer->addGeometry(pt);
    }
}

void MainWindow::drawAirways()
{
    m_airwayLayer = new GeometryLayer("Airways", m_mapAdapter);
    m_mapController->addLayer(m_airwayLayer);

    QList<Point*> points;

    foreach (Airway *awy, m_airac->airways())
    {
        points.clear();

        foreach (Leg *leg, awy->legs())
        {
            Fix *start = leg->start();
            Fix *end = leg->end();

            if (start != 0 && end != 0)
            {
                if (qAbs(end->longitude() - start->longitude()) > 180)
                {
                    LineString *ls = createAirway(points);
                    m_airwayLayer->addGeometry(ls);

                    points.clear();

                    continue;
                }

                points << new Point(start->longitude(), end->latitude());
            }
        }

        LineString *ls = createAirway(points);
        m_airwayLayer->addGeometry(ls);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_mapController->resize(event->size());
}

void MainWindow::createChilds()
{
    m_airportPixmap = new QPixmap("/home/menax/Projekte/local/nd-planner/prototyping/greendot.png");
    m_navaidPixmap = new QPixmap("/home/menax/Projekte/local/nd-planner/prototyping/bluedot.png");
    m_waypointPixmap = new QPixmap("/home/menax/Projekte/local/nd-planner/prototyping/reddot.png");

    m_legPeg = new QPen(QColor(0,0,255,100));
    m_legPeg->setWidth(1);

    m_airac = new Airac();
}

void MainWindow::createActions()
{
    action_loadAirac = new QAction(tr("&Load AIRAC..."), this);
    action_loadAirac->setStatusTip(tr("Loads an AIRAC file"));

    action_Quit = new QAction(tr("&Quit"), this);
    action_Quit->setShortcut(QKeySequence::Quit);
    action_Quit->setStatusTip(tr("Quit"));
}

void MainWindow::createMenus()
{
    menu_File->addAction(action_loadAirac);
    menu_File->addAction(action_Quit);
}

void MainWindow::makeConnections()
{
    connect(action_loadAirac, SIGNAL(triggered(bool)), this, SLOT(setAiracPath()));
    connect(action_Quit, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void MainWindow::createMap()
{
    m_mapController = new MapControl(this->size());
    m_mapAdapter = new OSMMapAdapter();
    m_groundLayer = new MapLayer("OpenStreetMap-Groundlayer", m_mapAdapter);

    m_mapController->addLayer(m_groundLayer);

    setCentralWidget(m_mapController);

    m_mapController->showScale(true);
    m_mapController->setZoom(2);
}

LineString* MainWindow::createAirway(const QList<Point*> &points)
{
    return new LineString(points, "", m_legPeg);
}
