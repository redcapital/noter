#ifndef QMLENGINE_H
#define QMLENGINE_H

#include <memory>
#include <QQmlApplicationEngine>
#include "repository.h"
#include "notelistmodel.h"

class QmlEngine : public QQmlApplicationEngine
{
	Q_OBJECT
private:
	std::unique_ptr<NoteListModel> listModel;

public:
	void initialize(Repository& repository);
};

#endif // QMLENGINE_H
