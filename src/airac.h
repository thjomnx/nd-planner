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
#include <QFile>

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
    ~Airac();

    static QString buildKey(const QString &identifier, const qreal latitude, const qreal longitude);

    QFile* file() const { return m_path; }

    QList<Airport*> airports() const { return m_airports; }
    QList<Navaid*> navaids() const { return m_navaids; }
    QList<Waypoint*> waypoints() const { return m_waypoints; }
    QList<Airway*> airways() const { return m_airways; }

public slots:
    void loadAirac(const QString &path);

private:
    void loadAiports(const QString &path);
    void loadNavaids(const QString &path);
    void loadWaypoints(const QString &path);
    void loadAirways(const QString &path);

    QFile *m_path;

    QHash<QString, Fix*> m_fixes;
    QList<Airport*> m_airports;
    QList<Navaid*> m_navaids;
    QList<Waypoint*> m_waypoints;
    QList<Airway*> m_airways;
};

#endif // AIRAC_H
