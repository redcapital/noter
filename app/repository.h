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
	bool connect(QString filepath, bool isExisting);
	std::vector<std::shared_ptr<Note>> findNotes(const QString& query);
	QString getLastError();
	virtual ~Repository();
};

#endif // REPOSITORY_H
