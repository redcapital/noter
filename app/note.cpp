#include <algorithm>
#include "note.h"
#include <QDebug>

using namespace std;

Note::Note(unsigned int id, unsigned int createdAt, unsigned int updatedAt, const QString& content) :
	id(id), createdAt(createdAt), updatedAt(updatedAt), content(content)
{
	tagList = new TagList(this);
}

QString Note::getTitle() const
{
	QString trimmed = this->content.trimmed();
	int cutPoint = trimmed.midRef(0, 40).indexOf("\n");
	if (cutPoint == -1) {
		cutPoint = 40;
	}
	return trimmed.mid(0, cutPoint);
}

bool Note::setContent(const QString &content)
{
	if (this->content != content) {
		this->dirty = true;
		this->content = content;
		return true;
	}
	return false;
}

void Note::setUpdatedAt(unsigned int updatedAt)
{
	if (this->updatedAt != updatedAt) {
		this->updatedAt = updatedAt;
	}
}

void Note::setTags(TagList* tags)
{
	if (tagList) {
		delete tagList;
	}
	tags->setParent(this);
	tagList = tags;
}

void Note::addTag(Tag* tag)
{
	tagList->addTag(tag);
}

void Note::removeTag(Tag *tag)
{
	tagList->removeTag(tag);
}

void Note::resetDirty()
{
	this->dirty = false;
}
