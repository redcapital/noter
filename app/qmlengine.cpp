#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>
#include "qmlengine.h"
#include "note.h"

void QmlEngine::initialize(Repository &repository)
{
	this->rootContext()->setContextProperty("repository", &repository);
	this->rootContext()->setContextProperty("textAreaBackend", &textAreaBackend);
	qmlRegisterType<Note>("com.github.galymzhan", 0, 1, "Note");
	qmlRegisterType<ResultSet>("com.github.galymzhan", 0, 1, "ResultSet");
	this->load(QUrl("qrc:/main.qml"));
}
