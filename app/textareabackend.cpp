#include "textareabackend.h"

void TextAreaBackend::setDocument(QQuickTextDocument *_document)
{
	document = _document->textDocument();
	highlighter.reset(new MarkdownHighlighter(document, 100));
}
