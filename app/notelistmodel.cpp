#include "notelistmodel.h"
#include <QDebug>

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
	if (!this->results || index.row() < 0 || index.row() >= this->results->size()) {
		return QVariant();
	}
	if (role == NoteListModel::TITLE) {
		return this->results->at(index.row()).getTitle();
	}
	if (role == NoteListModel::UPDATED) {
		return this->results->at(index.row()).getUpdatedAt();
	}
	return QVariant();
}

QVariant NoteListModel::get(int index) const
{
	if (!this->results || index < 0 || index >= this->results->size()) {
		return QVariant();
	}
	return this->results->at(index).getContent();
}

void NoteListModel::query(const QString &query)
{
	emit beginResetModel();
	this->results = this->repository->findNotes(query);
	emit endResetModel();
}

QHash<int, QByteArray> NoteListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[NoteListModel::TITLE] = "title";
	roles[NoteListModel::UPDATED] = "updatedAt";
	return roles;
}
