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

#include "airport.h"

#define AIRPORT_ID_IDX      1
#define AIRPORT_NAME_IDX    2
#define AIRPORT_LAT_IDX     3
#define AIRPORT_LON_IDX     4
#define AIRPORT_ELEV_IDX    5

Airport::Airport(QString identifier, QString name, qreal latitude, qreal longitude, qint32 elevation)
    : Fix(latitude, longitude)
{
    m_identifier = identifier;
    m_name = name;
    m_elevation = elevation;
}

Airport* Airport::parse(const QString &line)
{
    QStringList tokenList = line.split(',');

    QString identifier = tokenList[AIRPORT_ID_IDX].trimmed();
    QString name = tokenList[AIRPORT_NAME_IDX].trimmed();
    qreal latitude = tokenList[AIRPORT_LAT_IDX].toDouble();
    qreal longitude = tokenList[AIRPORT_LON_IDX].toDouble();
    qint32 elevation = tokenList[AIRPORT_ELEV_IDX].toInt();

    return new Airport(identifier, name, latitude, longitude, elevation);
}
