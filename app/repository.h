#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <memory>
#include <vector>
#include <QObject>
#include <QString>
#include "../sqlite/sqlite3.h"
#include "note.h"

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
	typedef std::shared_ptr<Note> NotePtr;
	typedef std::vector<NotePtr> ResultSet;
	typedef std::unique_ptr<ResultSet> ResultSetPtr;
	void disconnect();
	Q_INVOKABLE bool connect(QString filepath, bool isExisting);
	Q_INVOKABLE QString getLastError() const;
	Q_INVOKABLE bool updateNote(Note* note);
	Q_INVOKABLE bool createNote();
	Q_INVOKABLE bool deleteNote(Note* note);
	ResultSetPtr findNotes(const QString& query);
	virtual ~Repository();

signals:
	void noteCreated(NotePtr note);
	void noteUpdated(Note* note);
	void noteDeleted(Note* note);
};

#endif // REPOSITORY_H
