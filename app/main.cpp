#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "repositoryproxy.h"
#include "notelistmodel.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	Repository repository;
	RepositoryProxy repoProxy(&repository);
	NoteListModel noteListModel(&repository);
	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("repository", &repoProxy);
	engine.rootContext()->setContextProperty("noteListModel", &noteListModel);
	noteListModel.query("bla");
	engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

	return app.exec();
}
