#ifndef TAGLIST_H
#define TAGLIST_H

#include <vector>
#include <QObject>
#include "tag.h"

class TagList : public QObject
{
	Q_OBJECT
public:
	TagList(QObject* parent = 0) : QObject(parent) {}
	TagList(const std::vector<Tag*> _tags) : tags(_tags) {}
	void addTag(Tag* tag);
	void removeTag(Tag* tag);
	Q_INVOKABLE Tag* get(unsigned int index) const { return tags[index]; }
	Q_INVOKABLE unsigned int size() const { return tags.size(); }

private:
	std::vector<Tag*> tags;
};

#endif // TAGLIST_H
