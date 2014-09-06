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
	std::vector<std::shared_ptr<Note>> notes;

public:
	//NoteListModel() {}
	NoteListModel(Repository* repository) : repository(repository) {}
	int rowCount(const QModelIndex &parent) const { return this->notes.size(); }
	QVariant data(const QModelIndex &index, int role) const;
	void query(const QString& query);
};

#endif // NOTELISTMODEL_H
