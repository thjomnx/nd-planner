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
            if (buffer.last() == "SID" || buffer.last() == "STAR" || buffer.last() == "DCT" || airways.contains(buffer.last()))
            {
                buffer.append(it.next());
            }
            else
            {
                if (start == 0)
                {
                    start = fixes.value(buffer.at(0));
                }

                end = start->nearest(fixes.values(buffer.at(1)));

                Leg *leg = new Leg(start, end, Airac::distance(start, end));
                Segment *segment = new Segment(leg, Segment::DirectType);

                segments.append(segment);

                buffer.removeFirst();
            }
        }

        if (buffer.count() == 3)
        {
            if (!fixes.contains(buffer.last()))
            {
                qDebug() << "Unable to parse route:" << line;
                return 0;
            }
            else
            {
                if (start == 0)
                {
                    start = fixes.value(buffer.at(0));
                }

                Segment *segment = 0;

                if (buffer.at(1) == "SID" || buffer.at(1) == "STAR" || buffer.at(1) == "DCT")
                {
                    QList<Fix*> list = fixes.values(buffer.at(2));

                    if (list.count() == 0)
                    {
                        qDebug() << "No fixes found:" << buffer.at(2);
                        return 0;
                    }

                    end = start->nearest(list);
                    Leg *leg = new Leg(start, end, Airac::distance(start, end));

                    if (buffer.at(1) == "SID")
                    {
                        segment = new Segment(leg, Segment::DepartureType);
                    }
                    else if (buffer.at(1) == "STAR")
                    {
                        segment = new Segment(leg, Segment::ArrivalType);
                    }
                    else if (buffer.at(1) == "DCT")
                    {
                        segment = new Segment(leg, Segment::DirectType);
                    }
                }
                else
                {
                    QList<Fix*> list = fixes.values(buffer.at(2));
                    Airway *airway = 0;

                    if (list.count() == 0)
                    {
                        qDebug() << "No fixes found:" << buffer.at(2);
                        return 0;
                    }

                    foreach (Fix *fix, list)
                    {
                        airway = Airway::find(buffer.at(1), airways, start, fix);

                        if (airway != 0)
                        {
                            end = fix;
                            break;
                        }
                    }

                    if (airway == 0)
                    {
                        qDebug() << "Airway not found:" << buffer.at(1) << "[" << start->identifier() << "-->" << buffer.at(2) << "]";
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

                buffer.removeFirst();
                buffer.removeFirst();
            }
        }

        start = end;
        end = 0;
    }

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
