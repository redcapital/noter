#ifndef TEXTAREABACKEND_H
#define TEXTAREABACKEND_H

#include <memory>
#include <QObject>
#include <QTextDocument>
#include <QQuickTextDocument>
#include "highlighter.h"

class TextAreaBackend : public QObject
{
	Q_OBJECT
private:
	QTextDocument* document;
	std::unique_ptr<MarkdownHighlighter> highlighter;

public:
	Q_INVOKABLE void setDocument(QQuickTextDocument* _document);

};

#endif // TEXTAREABACKEND_H
