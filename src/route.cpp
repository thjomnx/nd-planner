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

#define ROUTE_DEPARTURE     "SID"
#define ROUTE_ARRIVAL       "STAR"
#define ROUTE_DIRECT        "DCT"

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

    QStringListIterator it(tokenList);
    QList<QString> buffer;

    Fix *start = 0;
    Fix *end = 0;

    while (it.hasNext())
    {
        if (buffer.count() < 2)
        {
            buffer.append(it.next());
        }

        if (buffer.count() == 2)
        {
            QString first = buffer.at(0).toUpper();
            QString last = buffer.at(1).toUpper();

            if (last == ROUTE_DEPARTURE || last == ROUTE_ARRIVAL || last == ROUTE_DIRECT || airways.contains(last))
            {
                buffer.append(it.next());
            }
            else
            {
                if (start == 0)
                {
                    if (fixes.contains(first))
                    {
                        start = fixes.value(first);
                    }
                    else
                    {
                        qDebug() << "Fix not found:" << first;
                        return 0;
                    }
                }

                if (fixes.contains(last))
                {
                    end = start->nearest(fixes.values(last));
                }
                else
                {
                    qDebug() << "No fixes found:" << last;
                    return 0;
                }

                Leg *leg = new Leg(start, end, Airac::distance(start, end));
                Segment *segment = new Segment(leg, Segment::DirectType);

                segments.append(segment);

                buffer.removeAt(0);
            }
        }

        if (buffer.count() == 3)
        {
            QString first = buffer.at(0).toUpper();
            QString middle = buffer.at(1).toUpper();
            QString last = buffer.at(2).toUpper();

            if (!fixes.contains(last))
            {
                qDebug() << "Unable to parse route:" << line;
                return 0;
            }
            else
            {
                if (start == 0)
                {
                    if (fixes.contains(first))
                    {
                        start = fixes.value(first);
                    }
                    else
                    {
                        qDebug() << "Fix not found:" << first;
                        return 0;
                    }
                }

                Segment *segment = 0;

                if (middle == ROUTE_DEPARTURE || middle == ROUTE_ARRIVAL || middle == ROUTE_DIRECT)
                {
                    QList<Fix*> list = fixes.values(last);

                    if (list.count() == 0)
                    {
                        qDebug() << "No fixes found:" << last;
                        return 0;
                    }

                    end = start->nearest(list);
                    Leg *leg = new Leg(start, end, Airac::distance(start, end));

                    if (middle == ROUTE_DEPARTURE)
                    {
                        segment = new Segment(leg, Segment::DepartureType);
                    }
                    else if (middle == ROUTE_ARRIVAL)
                    {
                        segment = new Segment(leg, Segment::ArrivalType);
                    }
                    else if (middle == ROUTE_DIRECT)
                    {
                        segment = new Segment(leg, Segment::DirectType);
                    }
                }
                else
                {
                    QList<Fix*> list = fixes.values(last);
                    Airway *airway = 0;

                    if (list.count() == 0)
                    {
                        qDebug() << "No fixes found:" << last;
                        return 0;
                    }

                    foreach (Fix *fix, list)
                    {
                        airway = Airway::find(middle, airways, start, fix);

                        if (airway != 0)
                        {
                            end = fix;
                            break;
                        }
                    }

                    if (airway == 0)
                    {
                        qDebug() << "Airway not found:" << middle << "[" << start->identifier() << "-->" << last << "]";
                        return 0;
                    }

                    QList<Leg*> legs = airway->legs(start, end);
                    segment = new Segment(legs, Segment::AirwayType, airway);
                }

                if (segment == 0)
                {
                    qDebug() << "Segment could not be created";
                    return 0;
                }

                segments.append(segment);

                buffer.removeAt(0);
                buffer.removeAt(0);
            }
        }

        start = end;
        end = 0;
    }

    return segments.count() > 0 ? new Route(segments) : 0;
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
