#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QObject>
#include <QDebug>
#include "taglist.h"

class Note : public QObject {
	Q_OBJECT
private:
	unsigned int id, createdAt, updatedAt;
	QString content;
	TagList* tagList;
	bool dirty = false;

public:
	Note() {}
	Note(unsigned int id, unsigned int createdAt, unsigned int updatedAt, const QString& content);
	Q_INVOKABLE unsigned int getId() const { return this->id; }
	Q_INVOKABLE unsigned int getCreatedAt() const { return this->createdAt; }
	Q_INVOKABLE unsigned int getUpdatedAt() const { return this->updatedAt; }
	Q_INVOKABLE QString getContent() const { return this->content; }
	Q_INVOKABLE bool setContent(const QString& content);
	Q_INVOKABLE QString getTitle() const;
	Q_INVOKABLE void setUpdatedAt(unsigned int updatedAt);

	// Note takes ownership of passed TagList object
	void setTags(TagList* tags);
	void addTag(Tag* tag);
	void removeTag(Tag* tag);
	// TagList is owned by Note, so the caller must not free the returned pointer
	Q_INVOKABLE TagList* getTags() const { return this->tagList; }
	bool isDirty() const { return this->dirty; }
	void resetDirty();
};

#endif // NOTE_H
