#ifndef TEXTAREABACKEND_H
#define TEXTAREABACKEND_H

#include <memory>
#include <QObject>
#include <QTextDocument>
#include <QQuickTextDocument>
#include <QMap>
#include <QVariant>
#include "highlighter.h"

class TextAreaBackend : public QObject
{
	Q_OBJECT
private:
	QTextDocument* document;
	std::unique_ptr<MarkdownHighlighter> highlighter;

public:
	Q_INVOKABLE void setDocument(QQuickTextDocument* _document);
	Q_INVOKABLE void setHighlightingRules(const QVariantMap& rules);
};

#endif // TEXTAREABACKEND_H
