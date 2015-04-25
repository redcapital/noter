#ifndef TAG_H
#define TAG_H

#include <QObject>
#include <QString>

class Tag : public QObject
{
	Q_OBJECT
public:
	Tag() {}
	explicit Tag(QObject *parent, int _id, const QString& _name);

	Q_PROPERTY(unsigned int tagId READ getId CONSTANT)
	Q_PROPERTY(QString name READ getName CONSTANT)

	unsigned int getId() const { return id; }
	QString getName() const { return name; }
	QString getNormalizedName() const { return normalizedName; }

	// Returns normalized version of the name
	static QString normalizeName(const QString& name);

	// partialName must be normalized
	bool nameStartsWith(const QString& partialName) const;

private:
	unsigned int id;
	QString name, normalizedName;
};

#endif // TAG_H
