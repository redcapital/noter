#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>
#include "qmlengine.h"
#include "note.h"
#include "resultset.h"
#include "taglist.h"

void QmlEngine::initialize(Repository &repository)
{
#ifdef QT_DEBUG
	this->rootContext()->setContextProperty("DEBUG", true);
#else
	this->rootContext()->setContextProperty("DEBUG", false);
#endif
	this->rootContext()->setContextProperty("repository", &repository);
	this->rootContext()->setContextProperty("textAreaBackend", &textAreaBackend);
	qmlRegisterType<Note>("com.github.galymzhan", 0, 1, "Note");
	qmlRegisterType<ResultSet>("com.github.galymzhan", 0, 1, "ResultSet");
	qmlRegisterType<TagList>("com.github.galymzhan", 0, 1, "TagList");
	qmlRegisterType<Tag>("com.github.galymzhan", 0, 1, "Tag");
	this->load(QUrl("qrc:/main.qml"));
}
