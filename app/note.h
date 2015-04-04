#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QObject>
#include <QDebug>

class Note : public QObject {
	Q_OBJECT
private:
	unsigned int id, createdAt, updatedAt;
	QString content;
	bool dirty = false;

public:
	Note() {}
	Note(unsigned int id, unsigned int createdAt, unsigned int updatedAt, const QString& content) :
		id(id), createdAt(createdAt), updatedAt(updatedAt), content(content) {}

	Q_INVOKABLE unsigned int getId() const { return this->id; }
	Q_INVOKABLE unsigned int getCreatedAt() const { return this->createdAt; }
	Q_INVOKABLE unsigned int getUpdatedAt() const { return this->updatedAt; }
	Q_INVOKABLE QString getContent() const { return this->content; }
	Q_INVOKABLE bool setContent(const QString& content);
	Q_INVOKABLE QString getTitle() const;
	Q_INVOKABLE void setUpdatedAt(unsigned int updatedAt);
	bool isDirty() const { return this->dirty; }
	void resetDirty();
};

#endif // NOTE_H
