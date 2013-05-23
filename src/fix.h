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

#ifndef FIX_H
#define FIX_H

#include <QObject>
#include <QList>

class Fix : public QObject
{
    Q_OBJECT

public:
    Fix();
    Fix(const Fix &other);
    explicit Fix(qreal latitude, qreal longitude);
    virtual ~Fix() { }

    static Fix* nearest(const Fix *mark, const QList<Fix*> &list);

    virtual QString identifier() const = 0;
    qreal latitude() const { return m_latitude; }
    qreal longitude() const { return m_longitude; }

protected:
    qreal m_latitude;
    qreal m_longitude;
};

#endif // FIX_H
