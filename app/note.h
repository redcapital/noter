#ifndef NOTE_H
#define NOTE_H

#include <QString>

class Note {
private:
	unsigned int id, createdAt, updatedAt;
	QString content;

public:
	Note(unsigned int id, unsigned int createdAt, unsigned int updatedAt, const QString& content) :
		id(id), createdAt(createdAt), updatedAt(updatedAt), content(content) {}

	unsigned int getId() const { return this->id; }
	unsigned int getCreatedAt() const { return this->createdAt; }
	unsigned int getUpdatedAt() const { return this->updatedAt; }
	QString getContent() const { return this->content; }
	QString getTitle() const;
};

#endif // NOTE_H
