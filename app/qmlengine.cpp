#include <QQmlContext>
#include <QQmlEngine>
#include <QtQml>
#include <QUrl>
#include "qmlengine.h"
#include "actions.h"
#include "note.h"

void QmlEngine::initialize(Repository &repository)
{
	this->listModel.reset(new NoteListModel(repository));
	this->rootContext()->setContextProperty("repository", &repository);
	this->rootContext()->setContextProperty("noteListModel", this->listModel.get());
	this->rootContext()->setContextProperty("textAreaBackend", &textAreaBackend);
	qmlRegisterType<Note>("com.github.galymzhan", 0, 1, "Note");
	qmlRegisterType<Actions>("com.github.galymzhan", 0, 1, "Actions");
	this->addImportPath("qrc:/qml-extras/modules");
	this->addImportPath("qrc:/qml-material/modules");
	this->load(QUrl("qrc:/main.qml"));
}
