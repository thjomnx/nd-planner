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

#ifndef SEGMENT_H
#define SEGMENT_H

#include <QObject>
#include <QList>

class Fix;
class Airway;
class Leg;

class Segment : public QObject
{
    Q_OBJECT

public:
    enum SegmentType
    {
        UnknownType,
        DirectType,
        AirwayType,
        DepartureType,
        ArrivalType
    };

    explicit Segment(Leg *leg, SegmentType type = UnknownType, Airway *airway = 0);
    explicit Segment(QList<Leg*> legs, SegmentType type = UnknownType, Airway *airway = 0);
    virtual ~Segment() { }

    QList<Leg*> legs() const { return m_legs; }
    Fix *start() const;
    Fix *end() const;

    SegmentType type() const { return m_type; }
    void setType(const SegmentType &type) { m_type = type; }

    Airway* airway() const { return m_airway; }
    qreal distance() const;

private:
    QList<Leg*> m_legs;
    SegmentType m_type;
    Airway *m_airway;
};

#endif // SEGMENT_H
