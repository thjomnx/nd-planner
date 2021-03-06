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

#ifndef LEG_H
#define LEG_H

#include <QObject>
#include <QMultiHash>

#include "fix.h"

class Airway;

class Leg : public QObject
{
    Q_OBJECT

public:
    explicit Leg(Fix *start, Fix *end, qreal distance);
    virtual ~Leg() { };

    static Leg* parse(const QString &line, QMultiHash<QString, Fix*> &fixes);

    Fix* start() const { return m_start; }
    Fix* end() const { return m_end; }
    qreal distance() const { return m_distance; }

private:
    Fix *m_start;
    Fix *m_end;
    qreal m_distance;
};

#endif // LEG_H
