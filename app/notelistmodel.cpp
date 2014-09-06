#include "notelistmodel.h"
#include <QDebug>

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
	qDebug() << " data req " << index.row() << ", " << role;
	if (index.row() < 0 || index.row() >= this->notes.size()) {
		return QVariant();
	}
	if (role == Qt::DisplayRole) {
		return this->notes.at(index.row())->getTitle();
	}
	return QVariant();
}

void NoteListModel::query(const QString &query)
{
	this->notes = this->repository->findNotes(query);
}
