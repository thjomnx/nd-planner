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

#ifndef AIRPORT_H
#define AIRPORT_H

#include <QObject>
#include <QHash>

#include "fix.h"

class Airport : public Fix
{
    Q_OBJECT

public:
    explicit Airport(QString identifier, QString name, qreal latitude, qreal longitude, qint32 elevation);
    ~Airport() { }

    static Airport* parse(const QString &line);
    static Airport* find(const QString &identifier, const QHash<QString, Airport*> &list);

    virtual QString identifier() const { return m_identifier; }
    virtual QString name() const { return m_name; }
    virtual qint32 elevation() const { return m_elevation; }

private:
    QString m_identifier;
    QString m_name;
    qint32 m_elevation;
};

#endif // AIRPORT_H
