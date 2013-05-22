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

#include <QStringList>

#include "waypoint.h"

#define WAYPOINT_ID_IDX    0
#define WAYPOINT_LAT_IDX   1
#define WAYPOINT_LON_IDX   2

Waypoint::Waypoint(QString identifier, qreal latitude, qreal longitude)
    : Fix(latitude, longitude)
{
    m_identifier = identifier;
}

Waypoint::~Waypoint()
{
}

Waypoint* Waypoint::parse(const QString &line)
{
    QStringList tokenList = line.split(',');

    QString identifier = tokenList[WAYPOINT_ID_IDX].trimmed();
    qreal latitude = tokenList[WAYPOINT_LAT_IDX].toDouble();
    qreal longitude = tokenList[WAYPOINT_LON_IDX].toDouble();

    return new Waypoint(identifier, latitude, longitude);
}

Waypoint *Waypoint::find(const QString &identifier, const QList<Waypoint*> &list)
{
    Waypoint *result = 0;

    foreach (Waypoint *wp, list)
    {
        if (wp->identifier() == identifier)
        {
            result = wp;
            break;
        }
    }

    return result;
}
