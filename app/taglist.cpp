#include <algorithm>
#include "taglist.h"
#include <QDebug>

void TagList::addTag(Tag *tag)
{
	auto it = std::find(tags.begin(), tags.end(), tag);
	if (it == tags.end()) {
		tags.push_back(tag);
	}
}

void TagList::removeTag(Tag *tag)
{
	auto it = std::find(tags.begin(), tags.end(), tag);
	if (it != tags.end()) {
		tags.erase(it);
	}
}
