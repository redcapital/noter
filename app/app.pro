TEMPLATE = app

LIBS += ../sqlite/libsqlite.a
TARGET = ../noter

QT += qml quick

LIBS += -stdlib=libc++
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11

# Stupid MacOS "deployment target" error
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
QMAKE_LFLAGS += -mmacosx-version-min=10.7
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

HEADERS += \
	repository.h \
	exception.h \
		notelistmodel.h \
    note.h \
    app.h \
    qmlengine.h

SOURCES += \
	repository.cpp \
	main.cpp \
    notelistmodel.cpp \
    note.cpp \
    app.cpp \
    qmlengine.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =
