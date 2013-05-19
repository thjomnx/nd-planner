# User variables
name = qmapcontrol

# Target settings
TARGET = $${name}
TEMPLATE = lib
DESTDIR = ../../lib
VERSION = 0.9.5.2

# Qt settings
QT += core network widgets

# Build settings
MOC_DIR = ../../build/$${name}/moc
OBJECTS_DIR = ../../build/$${name}

HEADERS += *.h
SOURCES += *.cpp
