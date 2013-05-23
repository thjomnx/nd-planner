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

#include "airway.h"
#include "leg.h"

#define AIRWAY_ID_IDX      1

Airway::Airway(QString &identifier) : QObject()
{
    m_identifier = identifier;
    m_type = parseType(identifier);
}

Airway::Airway(const Airway &other) : QObject()
{
    m_identifier = other.identifier();
    m_legs = other.legs();
    m_type = other.type();
}

Airway* Airway::parse(const QString &line)
{
    QStringList tokenList = line.split(',');

    QString identifier = tokenList[1].trimmed();

    return new Airway(identifier);
}

Airway* Airway::find(const QString &identifier, const QMultiHash<QString, Airway*> &mhash, Fix *start, Fix *end)
{
    Airway *result = 0;
    bool hit;

    foreach (Airway *awy, mhash.values(identifier))
    {
        hit = false;

        foreach (Leg *leg, awy->legs())
        {
            if (!hit)
            {
                if (leg->start() == start)
                {
                    hit = true;
                }
            }

            if (hit)
            {
                if (leg->end() == end)
                {
                    result = awy;
                    break;
                }
            }
        }
    }

    return result;
}

bool Airway::exists(const QString &identifier, const QMultiHash<QString, Airway*> &mhash)
{
    return mhash.values(identifier).count() > 0;
}

QList<Leg*> Airway::legs(const Fix *start, const Fix *end)
{
    QList<Leg*> result;
    bool hit = false;

    foreach (Leg *leg, m_legs)
    {
        if (leg->start() == start)
        {
            hit = true;
        }

        if (hit)
        {
            result.append(leg);
        }

        if (leg->end() == end)
        {
            break;
        }
    }

    return result;
}

Fix* Airway::entry() const
{
    return (m_legs.first() != 0) ? m_legs.first()->start() : 0;
}

Fix* Airway::exit() const
{
    return (m_legs.last() != 0) ? m_legs.last()->end() : 0;
}

void Airway::append(Leg *leg)
{
    m_legs.append(leg);
}

Airway::AirwayType Airway::parseType(const QString &identifier)
{
    AirwayType type = UnknownType;

    if (identifier.startsWith('Q') || identifier.startsWith('U'))
    {
        type = HighAltitudeType;
    }
    else if (identifier.startsWith('T') || identifier.startsWith('V'))
    {
        type = LowAltitudeType;
    }

    return type;
}
