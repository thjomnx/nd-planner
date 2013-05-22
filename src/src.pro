#
# This file is part of 'nd-planner'.
#
#   'nd-planner' is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    'nd-planner' is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with 'nd-planner'. If not, see <http://www.gnu.org/licenses/>.
#

#
# Project build file (qmake) for 'nd-planner' main sources
#

# User variables
name = nd-planner

# Target settings
TARGET = $${name}
TEMPLATE = app
INCLUDEPATH += . ../ext/qmapcontrol
LIBS += -L../lib -lqmapcontrol

# Qt settings
QT += core network widgets
CONFIG += qt debug warn_on

# Build settings
DESTDIR = ../bin/$${name}
UI_DIR = ../build/$${name}/ui
MOC_DIR = ../build/$${name}/moc
RCC_DIR = ../build/$${name}/rcc
OBJECTS_DIR = ../build/$${name}

# Build definitions
#DEFINES += "DUMP_AIRAC"

# Main sources
HEADERS += *.h
SOURCES += *.cpp
FORMS += *.ui
