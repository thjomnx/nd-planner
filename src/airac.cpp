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

#include <cmath>

#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "airac.h"
#include "airport.h"
#include "navaid.h"
#include "waypoint.h"
#include "airway.h"
#include "leg.h"

Airac::Airac() : QObject()
{
    m_path = new QFile(this);
}

Airac::~Airac()
{
}

// TODO Move this thing to a utility class
qreal Airac::distance(const Fix *f1, const Fix *f2)
{
    const qreal EARTH_RADIUS_NM = 3438.2;

    qreal lat1 = f1->latitude() * (M_PI / 180);
    qreal lon1 = f1->longitude() * (M_PI / 180);
    qreal lat2 = f2->latitude() * (M_PI / 180);
    qreal lon2 = f2->longitude() * (M_PI / 180);

    qreal dlon = lon2 - lon1;
    qreal dlat = lat2 - lat1;

    qreal a = pow(sin(dlat / 2), 2.0) + cos(f1->latitude() * (M_PI / 180)) * cos(f2->latitude() * (M_PI / 180)) * pow(sin(dlon / 2), 2.0);

    return EARTH_RADIUS_NM * 2 * atan2(sqrt(a), sqrt(1 - a));
}

void Airac::loadAirac(const QString &path)
{
    m_path->setFileName(path);

    qDebug() << "Loading AIRAC";

    loadWaypoints(m_path->fileName() + "/Waypoints.txt");
    loadNavaids(m_path->fileName() + "/Navaids.txt");
    loadAirports(m_path->fileName() + "/Airports.txt");
    loadAirways(m_path->fileName() + "/ATS.txt");

    qDebug() << "AIRAC loaded";
}

void Airac::loadAirports(const QString &path)
{
    QFile airports(path);

    if (!airports.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open file:" << airports.fileName();
        return;
    }

    qDebug() << "Loading airports into memory";

#ifdef DUMP_AIRAC
    QFile dumpFile("loadAirports.dump");
    dumpFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream dumpOut(&dumpFile);

    quint64 dumpCnt = 0;
#endif

    m_airports.clear();
    m_airports.reserve(15000);

    QTextStream in(&airports);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith('A'))
        {
            Airport *ap = Airport::parse(line);

#ifdef DUMP_AIRAC
            dumpOut << ap->identifier() << " --> " << ap << endl;
            dumpCnt++;
#endif

            m_fixes.insert(ap->identifier(), ap);
            m_airports.insert(ap->identifier(), ap);
        }
    }

    m_airports.squeeze();

#ifdef DUMP_AIRAC
    dumpOut << endl << dumpCnt << " airports loaded" << endl;
    dumpOut << m_airports.count() << " airports in list" << endl;
    dumpOut << m_fixes.count() << " fixes in hashmap" << endl;

    dumpFile.close();
#endif

    qDebug() << "Airports loaded";
}

void Airac::loadNavaids(const QString &path)
{
    QFile navaids(path);

    if (!navaids.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open file:" << navaids.fileName();
        return;
    }

    qDebug() << "Loading navaids into memory";

#ifdef DUMP_AIRAC
    QFile dumpFile("loadNavaids.dump");
    dumpFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream dumpOut(&dumpFile);

    quint64 dumpCnt = 0;
#endif

    m_navaids.clear();
    m_navaids.reserve(16000);

    QTextStream in(&navaids);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        Navaid *nav = Navaid::parse(line);

#ifdef DUMP_AIRAC
        dumpOut << nav->identifier() << " --> " << nav << endl;
        dumpCnt++;
#endif

        m_fixes.insert(nav->identifier(), nav);
        m_navaids.insert(nav->identifier(), nav);
    }

    m_navaids.squeeze();

#ifdef DUMP_AIRAC
    dumpOut << endl << dumpCnt << " navaids loaded" << endl;
    dumpOut << m_navaids.count() << " navaids in list" << endl;
    dumpOut << m_fixes.count() << " fixes in hashmap" << endl;

    dumpFile.close();
#endif

    qDebug() << "Navaids loaded";
}

void Airac::loadWaypoints(const QString &path)
{
    QFile waypoints(path);

    if (!waypoints.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open file:" << waypoints.fileName();
        return;
    }

    qDebug() << "Loading waypoints into memory";

#ifdef DUMP_AIRAC
    QFile dumpFile("loadWaypoints.dump");
    dumpFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream dumpOut(&dumpFile);

    quint64 dumpCnt = 0;
#endif

    m_waypoints.clear();
    m_waypoints.reserve(230000);

    QTextStream in(&waypoints);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        Waypoint *wp = Waypoint::parse(line);

#ifdef DUMP_AIRAC
        dumpOut << wp->identifier() << " --> " << wp << endl;
        dumpCnt++;
#endif

        m_fixes.insert(wp->identifier(), wp);
        m_waypoints.insert(wp->identifier(), wp);
    }

    m_waypoints.squeeze();

#ifdef DUMP_AIRAC
    dumpOut << endl << dumpCnt << " waypoints loaded" << endl;
    dumpOut << m_waypoints.count() << " waypoints in list" << endl;
    dumpOut << m_fixes.count() << " fixes in hashmap" << endl;

    dumpFile.close();
#endif

    qDebug() << "Waypoints loaded";
}

void Airac::loadAirways(const QString &path)
{
    QFile airways(path);

    if (!airways.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open file:" << airways.fileName();
        return;
    }

    qDebug() << "Loading airways into memory";

#ifdef DUMP_AIRAC
    QFile dumpFile("loadAirways.dump");
    dumpFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream dumpOut(&dumpFile);

    quint64 dumpCnt = 0;
#endif

    m_airways.clear();
    m_airways.reserve(150000);

    QTextStream in(&airways);
    Airway *awy = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith('A'))
        {
            if (awy != 0)
            {
                m_airways.insert(awy->identifier(), awy);

#ifdef DUMP_AIRAC
                dumpOut << awy->identifier() << " --> " << awy << endl;

                foreach (Leg *leg, awy->legs())
                {
                    dumpOut << leg->start()->identifier() << "(" << leg->start() << ") --> ";
                    dumpOut << leg->end()->identifier() << "(" << leg->end() << ")" << endl;
                    dumpCnt++;
                }

                dumpOut << endl;
#endif
            }

            awy = Airway::parse(line);
        }
        else if (line.startsWith('S'))
        {
            Leg *leg = Leg::parse(line, m_fixes);

            if (awy != 0)
            {
                awy->append(leg);
            }
        }
    }

    if (awy != 0)
    {
        m_airways.insert(awy->identifier(), awy);
    }

    m_airways.squeeze();

#ifdef DUMP_AIRAC
    dumpOut << endl << dumpCnt << " airways loaded" << endl;
    dumpOut << m_airways.count() << " airways in list" << endl;

    dumpFile.close();
#endif

    qDebug() << "Airways loaded";
}
