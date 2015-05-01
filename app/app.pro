TEMPLATE = app

include(../common.pri)

LIBS += ../sqlite/build/libsqlite.a -licui18n -licuuc -licudata

TARGET = ../noter

INCLUDEPATH += ../pmh ../sqlite

QT += qml quick
CONFIG += c++11

macx {
	# Stupid MacOS "deployment target" error
	QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
	QMAKE_LFLAGS += -mmacosx-version-min=10.7
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}

HEADERS += \
	repository.h \
	note.h \
	app.h \
	qmlengine.h \
	textareabackend.h \
	highlighter.h \
	../pmh/pmh_parser.h \
	../pmh/pmh_definitions.h \
	resultset.h \
	tag.h \
	taglist.h \
	migrator.h

SOURCES += \
	repository.cpp \
	main.cpp \
	note.cpp \
	app.cpp \
	qmlengine.cpp \
	textareabackend.cpp \
	highlighter.cpp \
	../pmh/pmh_parser.c \
	resultset.cpp \
	tag.cpp \
	taglist.cpp \
	migrator.cpp

RESOURCES += \
	frontend/resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = frontend/
