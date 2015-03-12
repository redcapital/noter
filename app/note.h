#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QObject>

class Note : public QObject {
	Q_OBJECT
private:
	unsigned int id, createdAt, updatedAt;
	QString content;
	bool dirty = false;

public:
	Q_PROPERTY(unsigned int id READ getId)
	Q_PROPERTY(QString content READ getContent)
	Q_PROPERTY(QString title READ getTitle)
	Q_PROPERTY(unsigned int createdAt READ getCreatedAt)
	Q_PROPERTY(unsigned int updatedAt READ getUpdatedAt)

	Note() {}
	Note(unsigned int id, unsigned int createdAt, unsigned int updatedAt, const QString& content) :
		id(id), createdAt(createdAt), updatedAt(updatedAt), content(content) {}

	unsigned int getId() const { return this->id; }
	unsigned int getCreatedAt() const { return this->createdAt; }
	unsigned int getUpdatedAt() const { return this->updatedAt; }
	QString getContent() const { return this->content; }
	void setContent(const QString& content);
	void setUpdatedAt(unsigned int updatedAt);
	QString getTitle() const;
	bool isDirty() const { return this->dirty; }
	void resetDirty();
};

#endif // NOTE_H
