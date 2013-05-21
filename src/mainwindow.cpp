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
#include "route.h"
#include "segment.h"

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
//     drawAirways();

    Airport *eddl = Airport::find("EDDL", m_airac->airports());
    Airport *klax = Airport::find("KLAX", m_airac->airports());

    qDebug() << "Distance EDDL --> KLAX =" << Airac::distance(eddl, klax);
}

void MainWindow::setRoute()
{
    QString str = "EDDL SID DODEN Y853 BOMBI UL984 RASPU UZ660 ROKEM UZ650 NARKA UL140 REBLA UL620 DINRO L601 ";
    str += "ODERO UP975 SIV UW710 ARPUT UP975 KATUT UM688 DENKI UP975 ILMAP UM688 PEBAD UP975 SIDAD R784 ";
    str += "IMDAT B416 DURSI R784 EGMIT B416 ORSAR R784 PEBAT B416 DESDI STAR OMDB";

    Route *route = Route::parse(str, m_airac);

//     foreach (Leg *leg, route->legs())
//     {
//         qDebug() << leg->start()->identifier() << "-->" << leg->end()->identifier();
//     }
//
//     foreach (Segment *seg, route->segments())
//     {
//         qDebug() << seg->start();
//     }
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

        if (awy->legs().count() > 0)
        {
            Fix *entry = awy->entry();

            if (entry != 0)
            {
                points << new Point(entry->longitude(), entry->latitude());
            }

            foreach (Leg *leg, awy->legs())
            {
                Fix *start = leg->start();
                Fix *end = leg->end();

                if (start != 0 && end != 0)
                {
                    if (qAbs(end->longitude() - start->longitude()) > 180.0)
                    {
                        if (start->longitude() > 0.0)
                        {
                            points << new Point(end->longitude() + 360.0, end->latitude());
                        }
                        else if (start->longitude() < 0.0)
                        {
                            points << new Point(end->longitude() - 360.0, end->latitude());
                        }

                        LineString *ls = createAirway(awy, points);
                        m_airwayLayer->addGeometry(ls);

                        points.clear();

                        if (start->longitude() > 0.0)
                        {
                            points << new Point(start->longitude() - 360.0, start->latitude());
                        }
                        else if (start->longitude() < 0.0)
                        {
                            points << new Point(start->longitude() + 360.0, start->latitude());
                        }
                    }
                    else
                    {
                        points << new Point(end->longitude(), end->latitude());
                    }
                }
            }

            LineString *ls = createAirway(awy, points);
            m_airwayLayer->addGeometry(ls);
        }
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

    m_unknownAirwayPen = new QPen(QColor(255,128,0,100));
    m_unknownAirwayPen->setWidth(1);

    m_lowAirwayPen = new QPen(QColor(0,255,255,100));
    m_lowAirwayPen->setWidth(1);

    m_highAirwayPen = new QPen(QColor(0,0,255,100));
    m_highAirwayPen->setWidth(1);

    m_airac = new Airac();
}

void MainWindow::createActions()
{
    action_loadAirac = new QAction(tr("Load AIRAC..."), this);
    action_loadAirac->setStatusTip(tr("Loads an AIRAC file"));

    action_loadRoute = new QAction(tr("Load route..."), this);
    action_loadRoute->setStatusTip(tr("Loads a route description"));

    action_Quit = new QAction(tr("Quit"), this);
    action_Quit->setShortcut(QKeySequence::Quit);
    action_Quit->setStatusTip(tr("Quit"));
}

void MainWindow::createMenus()
{
    menu_File->addAction(action_loadAirac);
    menu_File->addAction(action_loadRoute);
    menu_File->addAction(action_Quit);
}

void MainWindow::makeConnections()
{
    connect(action_loadAirac, SIGNAL(triggered(bool)), this, SLOT(setAiracPath()));
    connect(action_loadRoute, SIGNAL(triggered(bool)), this, SLOT(setRoute()));
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

LineString* MainWindow::createAirway(Airway *airway, const QList<Point*> &points)
{
    QPen *pen;

    switch (airway->type())
    {
        case Airway::LowAltitudeType: pen = m_lowAirwayPen; break;
        case Airway::HighAltitudeType: pen = m_highAirwayPen; break;
        default: pen = m_unknownAirwayPen; break;
    }

    return new LineString(points, "", pen);
}
