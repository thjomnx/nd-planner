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

#include "navaid.h"

#define NAVAID_ID_IDX      0
#define NAVAID_NAME_IDX    1
#define NAVAID_FREQ_IDX    2
#define NAVAID_LAT_IDX     6
#define NAVAID_LON_IDX     7

Navaid::Navaid(QString identifier, QString name, qreal frequency, qreal latitude, qreal longitude)
    : Fix(latitude, longitude)
{
    m_identifier = identifier;
    m_name = name;
    m_frequency = frequency;
}

Navaid* Navaid::parse(const QString &line)
{
    QStringList tokenList = line.split(',');

    QString identifier = tokenList[NAVAID_ID_IDX].trimmed();
    QString name = tokenList[NAVAID_NAME_IDX].trimmed();
    qreal frequency = tokenList[NAVAID_FREQ_IDX].toDouble();
    qreal latitude = tokenList[NAVAID_LAT_IDX].toDouble();
    qreal longitude = tokenList[NAVAID_LON_IDX].toDouble();

    return new Navaid(identifier, name, frequency, latitude, longitude);
}
