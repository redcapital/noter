TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = sqlite app
app.depends = sqlite

# Default rules for deployment.
include(deployment.pri)
