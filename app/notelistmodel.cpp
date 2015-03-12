#include <cassert>
#include <algorithm>
#include "app.h"
#include "notelistmodel.h"
#include <QDebug>

using namespace std;

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
	if (!this->results || index.row() < 0 || index.row() >= this->results->size()) {
		return QVariant();
	}
	if (role == NoteListModel::TITLE) {
		return this->results->at(index.row())->getTitle();
	}
	if (role == NoteListModel::UPDATED) {
		return this->results->at(index.row())->getUpdatedAt();
	}
	return QVariant();
}

QHash<int, QByteArray> NoteListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[NoteListModel::TITLE] = "title";
	roles[NoteListModel::UPDATED] = "updatedAt";
	return roles;
}

Note* NoteListModel::get(unsigned int index) const
{
	if (!this->results || index >= this->results->size()) {
		return nullptr;
	}
	Note* note = this->results->at(index).get();
	assert(note);
	App::instance()->getQmlEngine().setObjectOwnership(note, QQmlEngine::CppOwnership);
	return note;
}

void NoteListModel::search(const QString &query)
{
	beginResetModel();
	this->results = this->repository.search(query);
	endResetModel();
}

void NoteListModel::create()
{
	if (!this->results) {
		this->results.reset(new Repository::ResultSet);
	}
	Repository::NotePtr note = this->repository.createNote();
	beginInsertRows(QModelIndex(), 0, 0);
	this->results->insert(this->results->begin(), note);
	endInsertRows();
}

void NoteListModel::update(Note* note, const QString& content)
{
	if (!this->results) {
		return;
	}
	if (note->getContent() == content) {
		return;
	}
	note->setContent(content);
	time_t now = time(nullptr);
	note->setUpdatedAt(now);
	int index = this->findIndex(note);
	if (index > 0) {
		beginMoveRows(QModelIndex(), index, index, QModelIndex(), 0);
		Repository::NotePtr updated = this->results->at(index);
		this->results->erase(this->results->begin() + index);
		this->results->insert(this->results->begin(), updated);
		endMoveRows();
		index = 0;
	}
	if (index >= 0) {
		auto modelIndex = createIndex(index, 0);
		QVector<int> roles{ NoteListModel::TITLE, NoteListModel::UPDATED };
		emit dataChanged(modelIndex, modelIndex, roles);
	}
}

void NoteListModel::deleteNote(Note* note)
{
	if (!this->results) {
		return;
	}
	this->repository.deleteNote(note);
	int index = this->findIndex(note);
	if (index >= 0) {
		beginRemoveRows(QModelIndex(), index, index);
		this->results->erase(this->results->begin() + index);
		endRemoveRows();
	}
}

int NoteListModel::findIndex(const Note* note)
{
	for (int i = 0; i < this->results->size(); i++) {
		if (this->results->at(i).get() == note) {
			return i;
		}
	}
	return -1;
}
