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

#include <QtTest/QtTest>

#include <QFile>
#include <QTextStream>

#include "airac.h"
#include "airway.h"
#include "leg.h"
#include "route.h"
#include "segment.h"

class RouteTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_Route_Valid_data();
    void test_Route_Valid();

    void test_Route_Invalid_data();
    void test_Route_Invalid();

private:
    void dump_Route(const QString &name, const Route *route);

    Airac m_airac;
};

void RouteTest::initTestCase()
{
    m_airac.loadAirac("/home/menax/Projekte/local/nd-planner/prototyping/RAMZZESS_B777_NATIVE_1304v2/X-Plane/Ramzzess B777");
}

void RouteTest::cleanupTestCase()
{
}

void RouteTest::test_Route_Valid_data()
{
    QTest::addColumn<QString>("route_line");
    QTest::addColumn<int>("route_segments");
    QTest::addColumn<int>("route_legs");

    QTest::newRow("EDDL-OMDB") << "EDDL SID DODEN Y853 BOMBI UL984 RASPU UZ660 ROKEM UZ650 NARKA UL140 REBLA UL620 DINRO L601 ODERO UP975 SIV UW710 ARPUT UP975 KATUT UM688 DENKI UP975 ILMAP UM688 PEBAD UP975 SIDAD R784 IMDAT B416 DURSI R784 EGMIT B416 ORSAR R784 PEBAT B416 DESDI STAR OMDB" << 22 << 61;
    QTest::newRow("EDDL-LPMA") << "EDDL DCT MODRU Z283 SUMAS UZ283 LNO UZ707 FAMEN UQ249 DISAK UQ237 LMG UN10 VELIN UN857 PPN UQ73 KORNO UA857 ABODA UN857 IBALU UA857 AMETA UN857 TOVRA UA857 VEDOD UN857 LZR UN871 GDV UN729 BIMBO DCT LPMA" << 18 << 30;
    QTest::newRow("CYYZ-EDDL") << "CYYZ SID DEDKI ART COVAN KB03E MIILS DOTTY MALOT GISTI DEXEN LAMSO UL603 BASNO UL620 PAM UP62 TEBRO STAR EDDL" << 14 << 17;
    QTest::newRow("OMDB-WMKK") << "OMDB SID RIKET M428 GOMTA TARBO MUNGA A777 VAXIM P307 PARAR N571 GUNIP R467 AGOSA STAR WMKK" << 9 << 26;
}

void RouteTest::test_Route_Valid()
{
    QFETCH(QString, route_line);
    QFETCH(int, route_segments);
    QFETCH(int, route_legs);

    Route *route = Route::parse(route_line, &m_airac);

    if (route != 0)
    {
        QCOMPARE(route->segments().count(), route_segments);
        QCOMPARE(route->legs().count(), route_legs);

        dump_Route(QTest::currentDataTag(), route);
    }
}

void RouteTest::test_Route_Invalid_data()
{
    QTest::addColumn<QString>("route_line");
    QTest::addColumn<Route*>("route_ptr");

    Route *ptr = 0;

    QTest::newRow("Empty route") << "" << ptr;
    QTest::newRow("Invalid airport") << "CYYZP SID DEDKI ART COVAN KB03E MIILS DOTTY MALOT GISTI DEXEN LAMSO UL603 BASNO UL620 PAM UP62 TEBRO STAR EDDL" << ptr;
    QTest::newRow("Invalid navaid") << "CYYZ SID DEDKI ART COVAN KB03E MIILS DOTTY MALOT GISTI DEXEN LAMSO UL603 BASNO UL620 PA% UP62 TEBRO STAR EDDL" << ptr;
    QTest::newRow("Invalid waypoint") << "CYYZ SID DEDKI ART COVAN KB03E MIILS DOTT MALOT GISTI DEXEN LAMSO UL603 BASNO UL620 PAM UP62 TEBRO STAR EDDL" << ptr;
    QTest::newRow("Invalid airway") << "CYYZ SID DEDKI ART COVAN KB03E MIILS DOTTY MALOT GISTI DEXEN LAMSO UL60333 BASNO UL620 PAM UP62 TEBRO STAR EDDL" << ptr;
    QTest::newRow("Doubled SID") << "CYYZ SID SID DEDKI ART COVAN KB03E MIILS DOTTY MALOT GISTI DEXEN LAMSO UL603 BASNO UL620 PAM UP62 TEBRO STAR EDDL" << ptr;
    QTest::newRow("Doubled STAR") << "CYYZ SID SID DEDKI ART COVAN KB03E MIILS DOTTY MALOT GISTI DEXEN LAMSO UL603 BASNO UL620 PAM UP62 TEBRO STAR STAR EDDL" << ptr;
    QTest::newRow("Junk") << "d tlzot9fm i 76zijwer 4ßdgjsdme 4poth988frnme fwpoj" << ptr;
}

void RouteTest::test_Route_Invalid()
{
    QFETCH(QString, route_line);
    QFETCH(Route*, route_ptr);

    QCOMPARE(Route::parse(route_line, &m_airac), route_ptr);
}

void RouteTest::dump_Route(const QString &name, const Route *route)
{
    if (route != 0)
    {
        QFile dumpFile(name + ".dump");
        dumpFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream dumpOut(&dumpFile);

        dumpOut << "Legs:" << endl << endl;

        foreach (Leg *leg, route->legs())
        {
            dumpOut << QString("%1 --> %2 (%3 nm)").arg(leg->start()->identifier())
                                                    .arg(leg->end()->identifier())
                                                    .arg(leg->distance()) << endl;
        }

        dumpOut << endl << "Segments:" << endl << endl;

        foreach (Segment *seg, route->segments())
        {
            if (seg->type() == Segment::AirwayType)
            {
                dumpOut << QString("%1 -%2-> %3 (%4 nm)").arg(seg->start()->identifier())
                                                            .arg(seg->airway()->identifier())
                                                            .arg(seg->end()->identifier())
                                                            .arg(seg->distance()) << endl;
            }
            else if (seg->type() == Segment::DepartureType)
            {
                dumpOut << QString("%1 -SID-> %2 (%3 nm)").arg(seg->start()->identifier())
                                                            .arg(seg->end()->identifier())
                                                            .arg(seg->distance()) << endl;
            }
            else if (seg->type() == Segment::ArrivalType)
            {
                dumpOut << QString("%1 -STAR-> %2 (%3 nm)").arg(seg->start()->identifier())
                                                            .arg(seg->end()->identifier())
                                                            .arg(seg->distance()) << endl;
            }
            else
            {
                dumpOut << QString("%1 --> %2 (%3 nm)").arg(seg->start()->identifier())
                                                        .arg(seg->end()->identifier())
                                                        .arg(seg->distance()) << endl;
            }
        }

        dumpOut << endl << "Overall route distance: " << route->distance() << " nm" << endl;
        dumpOut << "Overall route segments: " << route->segments().count() << endl;
        dumpOut << "Overall route legs: " << route->legs().count() << endl << endl;

        dumpFile.close();
    }
}

QTEST_MAIN(RouteTest)

#include "routetest.moc"
