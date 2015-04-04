#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <memory>
#include <vector>
#include <QObject>
#include <QString>
#include "sqlite3.h"
#include "note.h"
#include "resultset.h"

class Repository : public QObject {
	Q_OBJECT
private:
	sqlite3* database = nullptr;
	QString lastError;
	void configureConnection();
	bool createSchema();
	bool validateSchema();
	bool checkSqliteError(int error);

public:
	void disconnect();
	Q_INVOKABLE bool connect(QString filepath, bool isExisting);
	Q_INVOKABLE QString getLastError() const;
	Q_INVOKABLE Note* createNote();
	Q_INVOKABLE bool persistNote(Note* note);
	Q_INVOKABLE bool deleteNote(Note* note);
	Q_INVOKABLE ResultSet* search(const QString& query);
	virtual ~Repository();
};

#endif // REPOSITORY_H
