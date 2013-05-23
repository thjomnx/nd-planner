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

#ifndef ROUTE_H
#define ROUTE_H

#include <QObject>

class Airac;
class Segment;
class Leg;

class Route : public QObject
{
    Q_OBJECT

public:
    explicit Route(QList<Segment*> segments);
    virtual ~Route() { }

    static Route* parse(const QString &line, Airac *airac);

    QList<Segment*> segments() const { return m_segments; }
    QList<Leg*> legs() const;
    qreal distance() const;

private:
    QList<Segment*> m_segments;
};

#endif // ROUTE_H
