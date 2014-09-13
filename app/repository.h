#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <memory>
#include <vector>
#include <QString>
#include "../sqlite/sqlite3.h"
#include "note.h"

class Repository {
private:
	sqlite3* database = nullptr;
	QString lastError;
	void configureConnection();
	bool createSchema();
	bool validateSchema();
	bool checkSqliteError(int error);

public:
	typedef std::vector<Note> ResultSet;
	typedef std::unique_ptr<ResultSet> ResultSetPtr;
	bool connect(QString filepath, bool isExisting);
	ResultSetPtr findNotes(const QString& query);
	QString getLastError();
	virtual ~Repository();
};

#endif // REPOSITORY_H
