TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = sqlite app
app.depends = sqlite

QT += qml quick

# Default rules for deployment.
include(deployment.pri)
