#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>
#include "qmlengine.h"
#include "note.h"

void QmlEngine::initialize(Repository &repository)
{
	this->listModel.reset(new NoteListModel(repository));
	this->rootContext()->setContextProperty("repository", &repository);
	this->rootContext()->setContextProperty("noteListModel", this->listModel.get());
	this->rootContext()->setContextProperty("textAreaBackend", &textAreaBackend);
	qmlRegisterType<Note>("com.github.galymzhan", 0, 1, "Note");
	this->load(QUrl("qrc:/main.qml"));
}
