#ifndef QMLENGINE_H
#define QMLENGINE_H

#include <memory>
#include <QQmlApplicationEngine>
#include "repository.h"
#include "notelistmodel.h"
#include "textareabackend.h"

class QmlEngine : public QQmlApplicationEngine
{
	Q_OBJECT
private:
	std::unique_ptr<NoteListModel> listModel;
	TextAreaBackend textAreaBackend;

public:
	void initialize(Repository& repository);
};

#endif // QMLENGINE_H
