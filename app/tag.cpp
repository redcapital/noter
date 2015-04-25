#include "tag.h"

Tag::Tag(QObject *parent, int _id, const QString& _name) : QObject(parent), id(_id), name(_name)
{
	normalizedName = normalizeName(name);
}

bool Tag::nameStartsWith(const QString &partialName) const
{
	return normalizedName.startsWith(partialName, Qt::CaseSensitive);
}

QString Tag::normalizeName(const QString& name)
{
	return name.trimmed().toCaseFolded();
}
