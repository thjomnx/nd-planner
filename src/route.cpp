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
#include "airway.h"
#include "leg.h"
#include "segment.h"

Route::Route(QList<Segment*> segments)
    : QObject()
{
    m_segments = segments;
}

Route* Route::parse(const QString &line, Airac *airac)
{
    QMultiHash<QString, Fix*> fixes = airac->fixes();
    QMultiHash<QString, Airway*> airways = airac->airways();

    QList<Segment*> segments;

    QStringList tokenList = line.trimmed().split(' ');

    // Parse departure segment
    Fix *origin = fixes.value(tokenList.first());
    Segment::SegmentType type = Segment::DirectType;

    tokenList.removeFirst();

    if (tokenList.first() == "SID")
    {
        tokenList.removeFirst();
        type = Segment::DepartureType;
    }
    else if (tokenList.first() == "DCT")
    {
        tokenList.removeFirst();
    }

    Fix *dfix = Fix::nearest(origin, fixes.values(tokenList.first()));
    Leg *leg = new Leg(origin, dfix, Airac::distance(origin, dfix));
    Segment *dep = new Segment(leg, type);

    // Parse arrival segment
    Fix *final = fixes.value(tokenList.last());
    type = Segment::DirectType;

    tokenList.removeLast();

    if (tokenList.last() == "STAR")
    {
        tokenList.removeLast();
        type = Segment::ArrivalType;
    }
    else if (tokenList.last() == "DCT")
    {
        tokenList.removeLast();
    }

    Fix *afix = Fix::nearest(final, fixes.values(tokenList.last()));
    leg = new Leg(afix, final, Airac::distance(afix, final));
    Segment *arr = new Segment(leg, type);

    // Append departure segment
    segments.append(dep);

    // Append enroute segments
    QStringListIterator it(tokenList);
    it.next();

    Fix *start = dep->end();
    Fix *end = 0;

    QString current, preview;
    QList<Fix*> list;
    bool isDirect = false;

    while (it.hasNext())
    {
        current.clear();
        preview.clear();
        list.clear();
        isDirect = false;

        current = it.next();
        isDirect = current == "DCT" || !Airway::exists(current, airways);

        if (isDirect)
        {
            current = it.next();
            list = fixes.values(current);

            if (list.count() > 0)
            {
                end = Fix::nearest(start, list);
            }
            else
            {
                qDebug() << "Fix not found:" << current;
                return 0;
            }

            Leg *leg = new Leg(start, end, Airac::distance(start, end));
            Segment *segment = new Segment(leg, Segment::AirwayType);

            segments.append(segment);
        }
        else
        {
            preview = it.peekNext();
            list = fixes.values(preview);

            if (list.count() == 0)
            {
                qDebug() << "No fixes found:" << preview;
                return 0;
            }

            Airway *airway = 0;

            foreach (Fix *fix, list)
            {
                airway = Airway::find(current, airways, start, fix);

                if (airway != 0)
                {
                    end = fix;
                    break;
                }
            }

            if (airway == 0)
            {
                qDebug() << "Airway not found:" << current << "[" << start->identifier() << "-->" << preview << "]";
                return 0;
            }

            QList<Leg*> list = airway->legs(start, end);
            Segment *segment = new Segment(list, Segment::AirwayType, airway);

            segments.append(segment);

            it.next();
        }

        start = end;
        end = 0;
    }

    // Append arrival segment
    segments.append(arr);

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

qreal Route::distance() const
{
    qreal dist = 0.0;

    foreach (Leg *leg, this->legs())
    {
        dist += leg->distance();
    }

    return dist;
}
