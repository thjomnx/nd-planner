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
#include <QStringList>

#include "route.h"
#include "airac.h"
#include "airport.h"
#include "waypoint.h"
#include "airway.h"
#include "leg.h"
#include "segment.h"

Route::Route(QList<Segment*> legs) : QObject()
{
    m_segments = legs;
}

Route::~Route()
{
}

Route* Route::parse(const QString &line, Airac *airac)
{
    QList<Segment*> segments;
    Leg *dep;
    Leg *arr;

    QStringList tokenList = line.trimmed().split(' ');

    foreach (QString t, tokenList)
    {
        qDebug() << t;
    }

    // Parse departure segment
    Airport *origin = Airport::find(tokenList.first(), airac->airports());
    tokenList.removeFirst();

    if (tokenList.first() == "SID")
    {
        tokenList.removeFirst();

        Waypoint *wp = Waypoint::find(tokenList.first(), airac->waypoints());
        dep = new Leg(origin, wp, 0.0);
    }
    else
    {
        Waypoint *wp = Waypoint::find(tokenList.first(), airac->waypoints());
        dep = new Leg(origin, wp, 0.0);
    }

    // Parse arrival segment
    Airport *final = Airport::find((tokenList.last()), airac->airports());
    tokenList.removeLast();

    if (tokenList.last() == "STAR")
    {
        tokenList.removeLast();

        Waypoint *wp = Waypoint::find(tokenList.last(), airac->waypoints());
        arr = new Leg(wp, final, 0.0);
    }
    else
    {
        Waypoint *wp = Waypoint::find(tokenList.last(), airac->waypoints());
        arr = new Leg(wp, final, 0.0);
    }

    // Append departure segment
    segments.append(new Segment(dep, 0, Segment::DepartureType));

    // Append enroute segments
    QStringListIterator it(tokenList);

    while (it.hasNext())
    {
        QString current = it.next();
        QString preview = "";
        bool hasAirway = false;

        Fix *start = Fix::find(current, airac->fixes().values());

        if (start == 0)
        {
            qDebug() << "Fix not found:" << current;
            return 0;
        }

        current = it.next();
        hasAirway = Airway::isAirway(current, airac->airways());

        if (hasAirway)
        {
            preview = it.peekNext();
            Fix *end = Fix::find(preview, airac->fixes().values());

            if (end == 0)
            {
                qDebug() << "Fix not found:" << preview;
                return 0;
            }

            Airway *airway = Airway::find(current, airac->airways(), start, end);

            if (airway == 0)
            {
                qDebug() << "Airway not found:" << current << "[" << start->identifier() << "-->" << end->identifier() << "]";

                return 0;
            }

            QList<Leg*> list = airway->findAll(start, end);
            Segment *segment = new Segment(list, airway, Segment::EnrouteType);

            segments.append(segment);
        }
        else
        {
            Fix *end = Fix::find(current, airac->fixes().values());

            if (end == 0)
            {
                qDebug() << "Fix not found:" << current;
                return 0;
            }

            Leg *leg = new Leg(start, end, 0.0);
            Segment *segment = new Segment(leg, 0, Segment::EnrouteType);

            segments.append(segment);
        }
    }

    // Append arrival segment
    segments.append(new Segment(arr, 0, Segment::ArrivalType));

    return new Route(segments);
}

QList<Leg*> Route::legs() const
{
    QList<Leg*> list;

    foreach (Segment *seg, m_segments)
    {
        list.append(seg->legs());
    }

    return list;
}
