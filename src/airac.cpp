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

#include <QDebug>
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

QString Airac::buildKey(const QString &identifier, const qreal latitude, const qreal longitude)
{
    return QString("%1_%2_%3").arg(identifier).arg(latitude).arg(longitude);
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

    QTextStream in(&airports);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith('A'))
        {
            Airport *ap = Airport::parse(line);

            m_fixes.insert(buildKey(ap->identifier(), ap->latitude(), ap->longitude()), ap);
            m_airports.append(ap);
        }
    }

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

    QTextStream in(&navaids);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        Navaid *nav = Navaid::parse(line);

        m_fixes.insert(buildKey(nav->identifier(), nav->latitude(), nav->longitude()), nav);
        m_navaids.append(nav);
    }

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

    QTextStream in(&waypoints);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        Waypoint *wp = Waypoint::parse(line);

        m_fixes.insert(buildKey(wp->identifier(), wp->latitude(), wp->longitude()), wp);
        m_waypoints.append(wp);
    }

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

    QTextStream in(&airways);
    Airway *awy = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith('A'))
        {
            if (awy != 0)
            {
                m_airways.append(awy);
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

    qDebug() << "Airways loaded";
}
