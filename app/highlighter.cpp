#include <utility>
#include <algorithm>
#include <QTextBlock>
#include <QHash>
#include <QDebug>
#include "highlighter.h"

WorkerThread::~WorkerThread()
{
	if (result != NULL) {
		pmh_free_elements(result);
	}
	free(content);
}

void WorkerThread::run()
{
	if (content == NULL) {
		return;
	}
	pmh_markdown_to_elements(content, pmh_EXT_NONE, &result);
}


MarkdownHighlighter::MarkdownHighlighter(QTextDocument *parent, int aWaitInterval) : QObject(parent), waitInterval(aWaitInterval)
{
	workerThread = NULL;
	cached_elements = NULL;
	timer = new QTimer(this);
	timer->setSingleShot(true);
	timer->setInterval(aWaitInterval);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
	document = parent;
	connect(document, SIGNAL(contentsChange(int,int,int)), this, SLOT(handleContentsChange(int,int,int)));
	this->parse();
}

void MarkdownHighlighter::setStyles(QVector<HighlightingStyle> &styles)
{
	this->highlightingStyles = styles;
}

void MarkdownHighlighter::clearFormatting()
{
	QTextBlock block = document->firstBlock();
	while (block.isValid()) {
		block.layout()->clearAdditionalFormats();
		QTextCursor cursor(block);
		QTextBlockFormat format = cursor.blockFormat();
		if (true || format.lineHeightType() != QTextBlockFormat::ProportionalHeight) {
			format.setLineHeight(130, QTextBlockFormat::ProportionalHeight);
			cursor.setBlockFormat(format);
		}
		block = block.next();
	}
}

bool MarkdownHighlighter::formatLessThan(const QTextLayout::FormatRange &r1, const QTextLayout::FormatRange &r2)
{
	return std::make_pair(r1.start, r1.length) < std::make_pair(r2.start, r2.length);
}


void MarkdownHighlighter::highlight()
{
	if (cached_elements == NULL) {
		qDebug() << "cached_elements is NULL";
		return;
	}
	static int cnt = 0;
	qDebug() << "highlight called" << cnt++;

	QHash<int, QList<QTextLayout::FormatRange> > lists;
	QHash<int, QList<QTextLayout::FormatRange> >::iterator hashIter;
	QList<QTextBlock> blocks;

	this->clearFormatting();

	for (int i = 0; i < highlightingStyles.size(); i++) {
		HighlightingStyle style = highlightingStyles.at(i);
		pmh_element *elem_cursor = cached_elements[style.type];
		while (elem_cursor != NULL) {
			if (elem_cursor->end <= elem_cursor->pos) {
				elem_cursor = elem_cursor->next;
				continue;
			}

			// "The QTextLayout object can only be modified from the
			// documentChanged implementation of a QAbstractTextDocumentLayout
			// subclass. Any changes applied from the outside cause undefined
			// behavior." -- we are breaking this rule here. There might be
			// a better (more correct) way to do this.

			int startBlockNum = document->findBlock(elem_cursor->pos).blockNumber();
			int endBlockNum = document->findBlock(elem_cursor->end).blockNumber();
			for (int j = startBlockNum; j <= endBlockNum; j++) {
				QTextBlock block = document->findBlockByNumber(j);
				hashIter = lists.find(block.blockNumber());
				if (hashIter == lists.end()) {
					QList<QTextLayout::FormatRange> emptyList;
					hashIter = lists.insert(block.blockNumber(), emptyList);
					blocks.append(block);
				}
				int blockpos = block.position();
				QTextLayout::FormatRange r;
				r.format = style.format;

				if (j == startBlockNum) {
					r.start = elem_cursor->pos - blockpos;
					r.length = (startBlockNum == endBlockNum)
						? elem_cursor->end - elem_cursor->pos
						: block.length() - r.start;
				} else if (j == endBlockNum) {
					r.start = 0;
					r.length = elem_cursor->end - blockpos;
				} else {
					r.start = 0;
					r.length = block.length();
				}
				hashIter.value().append(r);
			}

			elem_cursor = elem_cursor->next;
		}
	}
	std::sort(blocks.begin(), blocks.end());
	for (auto &block : blocks) {
		hashIter = lists.find(block.blockNumber());
		std::sort(hashIter.value().begin(), hashIter.value().end(), &MarkdownHighlighter::formatLessThan);
		block.layout()->setAdditionalFormats(hashIter.value());
	}
	document->markContentsDirty(0, document->characterCount());
}

void MarkdownHighlighter::parse()
{
	if (workerThread != NULL && workerThread->isRunning()) {
		parsePending = true;
		return;
	}

	QString content = document->toPlainText();
	QByteArray ba = content.toUtf8();
	char *content_cstring = strdup((char *)ba.data());

	if (workerThread != NULL) {
		delete workerThread;
	}
	workerThread = new WorkerThread();
	workerThread->content = content_cstring;
	connect(workerThread, SIGNAL(finished()), this, SLOT(threadFinished()));
	parsePending = false;
	workerThread->start();
}

void MarkdownHighlighter::threadFinished()
{
	if (parsePending) {
		this->parse();
		return;
	}

	if (cached_elements != NULL) {
		pmh_free_elements(cached_elements);
	}
	cached_elements = workerThread->result;
	workerThread->result = NULL;

	this->highlight();
}

void MarkdownHighlighter::handleContentsChange(int position, int charsRemoved, int charsAdded)
{
	if (charsRemoved == 0 && charsAdded == 0) {
		return;
	}
	timer->stop();
	timer->start();
}

void MarkdownHighlighter::timerTimeout()
{
	this->parse();
}
