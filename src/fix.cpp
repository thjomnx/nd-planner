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

#include "fix.h"

Fix::Fix() : QObject()
{
    m_latitude = 0.0;
    m_longitude = 0.0;
}

Fix::Fix(const Fix &other) : QObject()
{
    m_latitude = other.latitude();
    m_longitude = other.longitude();
}

Fix::Fix(qreal latitude, qreal longitude) : QObject()
{
    m_latitude = latitude;
    m_longitude = longitude;
}

Fix::~Fix()
{
}

Fix* Fix::find(const QString &id, const QList<Fix*> &list)
{
    Fix *result = 0;

    foreach (Fix *fix, list)
    {
        if (fix->identifier() == id)
        {
            result = fix;
            break;
        }
    }

    return result;
}