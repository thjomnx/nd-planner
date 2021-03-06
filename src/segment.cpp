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
#include <QList>

#include "segment.h"
#include "leg.h"

Segment::Segment(Leg *leg, SegmentType type, Airway *airway)
    : QObject()
{
    m_legs.append(leg);
    m_airway = airway;
    m_type = type;
}

Segment::Segment(QList<Leg*> legs, SegmentType type, Airway *airway)
    : QObject()
{
    m_legs = legs;
    m_airway = airway;
    m_type = type;
}

Fix* Segment::start() const
{
    return (!m_legs.isEmpty()) ? m_legs.first()->start() : 0;
}

Fix* Segment::end() const
{
    return (!m_legs.isEmpty()) ? m_legs.last()->end() : 0;
}

qreal Segment::distance() const
{
    qreal dist = 0.0;

    foreach (Leg *leg, m_legs)
    {
        dist += leg->distance();
    }

    return dist;
}
