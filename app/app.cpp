#include "app.h"
#include <QDebug>

App::App(int argc, char* argv[]) :
QGuiApplication(argc, argv)
{
	this->repository.reset(new Repository);
	this->engine.reset(new QmlEngine);
	this->engine->initialize(*this->repository);
}
