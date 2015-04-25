#ifndef RESULTSET_H
#define RESULTSET_H

#include <QObject>
#include "sqlite3.h"
#include "note.h"
#include "repository.h"

class ResultSet : public QObject
{
	Q_OBJECT
private:
	Repository* repository;
	sqlite3_stmt* stmt = nullptr;
	Note* lastResult = nullptr;
	void doStep();

public:
	ResultSet() {}
	ResultSet(Repository* _repository, sqlite3_stmt* _stmt = nullptr);
	virtual ~ResultSet();
	Q_INVOKABLE bool hasMore();
	Q_INVOKABLE Note* fetch();

};

#endif // RESULTSET_H
