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

#ifndef AIRAC_H
#define AIRAC_H

#include <QObject>
#include <QHash>
#include <QMultiHash>

class QFile;

class Fix;
class Airport;
class Navaid;
class Waypoint;
class Airway;
class Leg;

class Airac : public QObject
{
    Q_OBJECT

public:
    Airac();
    virtual ~Airac();

    // TODO Move this thing to a utility class
    static qreal distance(const Fix *f1, const Fix *f2);

    QFile* file() const { return m_path; }

    const QMultiHash<QString, Fix*> fixes() const { return m_fixes; }
    const QHash<QString, Airport*> airports() const { return m_airports; }
    const QMultiHash<QString, Navaid*> navaids() const { return m_navaids; }
    const QMultiHash<QString, Waypoint*> waypoints() const { return m_waypoints; }
    const QMultiHash<QString, Airway*> airways() const { return m_airways; }

public slots:
    void loadAirac(const QString &path);

private:
    void loadAirports(const QString &path);
    void loadNavaids(const QString &path);
    void loadWaypoints(const QString &path);
    void loadAirways(const QString &path);

    QFile *m_path;

    QMultiHash<QString, Fix*> m_fixes;
    QHash<QString, Airport*> m_airports;
    QMultiHash<QString, Navaid*> m_navaids;
    QMultiHash<QString, Waypoint*> m_waypoints;
    QMultiHash<QString, Airway*> m_airways;
};

#endif // AIRAC_H
