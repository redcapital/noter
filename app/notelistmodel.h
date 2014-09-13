#ifndef NOTELISTMODEL_H
#define NOTELISTMODEL_H

#include <vector>
#include <QAbstractListModel>
#include "repository.h"
#include "note.h"

class NoteListModel : public QAbstractListModel {
	Q_OBJECT
private:
	Repository* repository;
	Repository::ResultSetPtr results;

public:
	enum Role {
		TITLE = 1,
		UPDATED = 2
	};

	NoteListModel(Repository* repository) : repository(repository) {}
	int rowCount(const QModelIndex &parent) const { return this->results ? this->results->size() : 0; }
	QVariant data(const QModelIndex &index, int role) const;
	QHash<int, QByteArray> roleNames() const;
	Q_INVOKABLE void query(const QString& query);
	Q_INVOKABLE QVariant get(int index) const;
};

#endif // NOTELISTMODEL_H
