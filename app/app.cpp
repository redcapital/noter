#include "app.h"
#include <QDebug>
#include <QSettings>

App::App(int argc, char* argv[]) :
QGuiApplication(argc, argv)
{
	this->setApplicationName("noter");
	this->setApplicationDisplayName("Noter");
	this->setOrganizationName("noter");
	QSettings::setDefaultFormat(QSettings::IniFormat);
	this->repository.reset(new Repository);
	this->engine.reset(new QmlEngine);
	this->engine->addImportPath("qrc:/");
	this->engine->initialize(*this->repository);
}
