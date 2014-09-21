#include <algorithm>
#include "note.h"
#include <QDebug>

using namespace std;

QString Note::getTitle() const
{
	QString trimmed = this->content.trimmed();
	int cutPoint = trimmed.midRef(0, 40).indexOf("\n");
	if (cutPoint == -1) {
		cutPoint = 40;
	}
	return trimmed.mid(0, cutPoint);
}

void Note::setContent(const QString &content)
{
	if (this->content != content) {
		this->dirty = true;
		this->content = content;
	}
}

void Note::setUpdatedAt(unsigned int updatedAt)
{
	if (this->updatedAt != updatedAt) {
		this->updatedAt = updatedAt;
	}
}

void Note::resetDirty()
{
	this->dirty = false;
}
