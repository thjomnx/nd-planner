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

#include "leg.h"
#include "airac.h"
#include "airway.h"

#define LEG_STARTID_IDX    1
#define LEG_STARTLAT_IDX   2
#define LEG_STARTLON_IDX   3
#define LEG_ENDID_IDX      4
#define LEG_ENDLAT_IDX     5
#define LEG_ENDLON_IDX     6
#define LEG_DIST_IDX       9

Leg::Leg(Fix *start, Fix *end, qreal distance)
    : QObject()
{
    m_start = start;
    m_end = end;
    m_distance = distance;
}

Leg* Leg::parse(const QString &line, QMultiHash<QString, Fix*> &fixes)
{
    QStringList tokenList = line.split(',');

    QString startId = tokenList[LEG_STARTID_IDX].trimmed();
    qreal startLat = tokenList[LEG_STARTLAT_IDX].toDouble();
    qreal startLon = tokenList[LEG_STARTLON_IDX].toDouble();

    QString endId = tokenList[LEG_ENDID_IDX].trimmed();
    qreal endLat = tokenList[LEG_ENDLAT_IDX].toDouble();
    qreal endLon = tokenList[LEG_ENDLON_IDX].toDouble();

    qreal distance = tokenList[LEG_DIST_IDX].toDouble();

    Fix *start = 0;
    Fix *end = 0;

    foreach (Fix *fix, fixes.values(startId))
    {
        if (fix->latitude() == startLat && fix->longitude() == startLon)
        {
            start = fix;
            break;
        }
    }

    foreach (Fix *fix, fixes.values(endId))
    {
        if (fix->latitude() == endLat && fix->longitude() == endLon)
        {
            end = fix;
            break;
        }
    }

    return new Leg(start, end, distance);
}
