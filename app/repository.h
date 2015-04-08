#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <memory>
#include <QHash>
#include <QObject>
#include <QString>
#include "sqlite3.h"
#include "note.h"
#include "taglist.h"

class ResultSet;

class Repository : public QObject {
	Q_OBJECT
private:
	sqlite3* database = nullptr;
	QString lastError;
	void configureConnection();
	bool createSchema();
	bool validateSchema();
	bool checkSqliteError(int error);
	void loadTags();
	typedef QHash<int, Tag*> TagTable;
	TagTable tags;

public:
	void disconnect();
	Q_INVOKABLE bool connect(QString filepath, bool isExisting);
	Q_INVOKABLE QString getLastError() const;

	Q_INVOKABLE Note* createNote();
	Q_INVOKABLE bool persistNote(Note* note);
	Q_INVOKABLE bool deleteNote(Note* note);
	Q_INVOKABLE ResultSet* search(const QString& query);

	Tag* getTagById(int id);
	Q_INVOKABLE Tag* createTag(const QString& name);
	Q_INVOKABLE TagList* autocompleteTag(const QString& name, const QList<int>& discardedIds);
	Q_INVOKABLE void addTag(Note* note, int tagId);
	Q_INVOKABLE void removeTag(Note* note, int tagId);

	virtual ~Repository();
};

#endif // REPOSITORY_H
