#ifndef RESULTSET_H
#define RESULTSET_H

#include <QObject>
#include "sqlite3.h"
#include "note.h"

class ResultSet : public QObject
{
	Q_OBJECT
private:
	sqlite3_stmt* stmt = nullptr;
	Note* lastResult;
	void doStep();

public:
	ResultSet() {}
	ResultSet(sqlite3_stmt* stmt_);
	virtual ~ResultSet();
	Q_INVOKABLE bool hasMore();
	Q_INVOKABLE Note* fetch();

};

#endif // RESULTSET_H
