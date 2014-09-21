#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include <vector>
#include <QAbstractListModel>
#include "repository.h"
#include "note.h"

class NoteListModel : public QAbstractListModel {
	Q_OBJECT
private:
	Repository& repository;
	Repository::ResultSetPtr results;
	int findIndex(const Note* note);

public:
	enum Role {
		TITLE = 1,
		UPDATED = 2
	};

	NoteListModel(Repository& repository);
	int rowCount(const QModelIndex &parent) const { return this->results ? this->results->size() : 0; }
	QVariant data(const QModelIndex &index, int role) const;
	QHash<int, QByteArray> roleNames() const;
	Q_INVOKABLE void query(const QString& query);
	Q_INVOKABLE Note* get(int index) const;

public slots:
	void onNoteCreated(Repository::NotePtr note);
	void onNoteUpdated(Note* note);
	void onNoteDeleted(Note* note);
};

#endif // NOTELISTMODEL_H
