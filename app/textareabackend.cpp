#include "textareabackend.h"

void TextAreaBackend::setDocument(QQuickTextDocument *_document)
{
	document = _document->textDocument();
	highlighter.reset(new MarkdownHighlighter(document, 100));
}

void TextAreaBackend::setHighlightingRules(const QVariantMap& rules)
{
	if (!highlighter) {
		return;
	}
	QVector<HighlightingStyle> styles;
	QHash<QString, QVector<pmh_element_type> > formatables;
	formatables.insert("header", { pmh_H1, pmh_H2, pmh_H3, pmh_H4, pmh_H5, pmh_H6 });
	formatables.insert("hrule", { pmh_HRULE });
	formatables.insert("list", { pmh_LIST_BULLET, pmh_LIST_ENUMERATOR });
	formatables.insert("link", { pmh_LINK, pmh_AUTO_LINK_URL, pmh_AUTO_LINK_EMAIL, pmh_REFERENCE });
	formatables.insert("image", { pmh_IMAGE });
	formatables.insert("code", { pmh_CODE, pmh_VERBATIM });
	formatables.insert("emphasis", { pmh_EMPH });
	formatables.insert("strong", { pmh_STRONG });
	formatables.insert("comment", { pmh_COMMENT });
	formatables.insert("blockquote", { pmh_BLOCKQUOTE });

	QHash<QString, QVector<pmh_element_type> >::const_iterator i = formatables.begin();
	while (i != formatables.constEnd()) {
		QVariant element(rules.value(i.key()));
		if (!element.isNull()) {
			QTextCharFormat elementFormat;
			QVariantMap elementRules(element.toMap());

			QVariant font(elementRules.value("fontFamily"));
			if (!font.isNull()) {
				elementFormat.setFontFamily(font.toString());
			}

			QVariant color(elementRules.value("color"));
			if (!color.isNull()) {
				elementFormat.setForeground(QBrush(QColor(color.toString())));
			}

			QVariant isBold = elementRules.value("bold");
			if (!isBold.isNull() && isBold.toBool()) {
				elementFormat.setFontWeight(QFont::Bold);
			}

			QVariant isItalic = elementRules.value("italic");
			if (!isItalic.isNull() && isItalic.toBool()) {
				elementFormat.setFontItalic(true);
			}

			for (auto &elementType : i.value()) {
				styles.append((HighlightingStyle){ elementType, elementFormat });
			}
		}
		++i;
	}

	highlighter->setStyles(styles);
}
