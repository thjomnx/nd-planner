#
# This file is part of 'telamon'.
#
#   'telamon' is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    'telamon' is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with 'telamon'. If not, see <http://www.gnu.org/licenses/>.
#

#
# Project build file (qmake) for 'telamon' testing sources
#

# User variables
name = nd-planner-test

# Qt settings
QT += testlib
INCLUDEPATH += ../src ../build/$${name}/moc

# Build settings
DESTDIR = ../bin/$${name}
MOC_DIR = ../build/$${name}/moc
OBJECTS_DIR = ../build/$${name}

# Unit test sources
SOURCES = units/routetest.cpp

# Unit sources
HEADERS += ../src/airac.h \
           ../src/fix.h \
           ../src/airport.h \
           ../src/navaid.h \
           ../src/waypoint.h \
           ../src/airway.h \
           ../src/leg.h \
           ../src/route.h \
           ../src/segment.h

SOURCES += ../src/airac.cpp \
           ../src/fix.cpp \
           ../src/airport.cpp \
           ../src/navaid.cpp \
           ../src/waypoint.cpp \
           ../src/airway.cpp \
           ../src/leg.cpp \
           ../src/route.cpp \
           ../src/segment.cpp
