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

#ifndef AIRWAY_H
#define AIRWAY_H

#include <QObject>

class Fix;
class Leg;

class Airway : public QObject
{
    Q_OBJECT

public:
    enum AirwayType
    {
        UnknownType,
        HighAltitudeType,
        LowAltitudeType
    };

    explicit Airway(QString &identifier);
    explicit Airway(const Airway &other);
    ~Airway();

    static Airway* parse(const QString &line);

    QString identifier() const { return m_identifier; }
    QList<Leg*> legs() const { return m_legs; }
    AirwayType type() const { return m_type; }
    Fix* start() const;
    Fix* end() const;

    void appendLeg(Leg *leg);

private:
    AirwayType parseType(const QString &identifier);

    QString m_identifier;
    QList<Leg*> m_legs;
    AirwayType m_type;
};

#endif // AIRWAY_H
