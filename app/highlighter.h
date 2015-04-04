/* Based on PEG Markdown Highlight project by Ali Rantakari */

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QTextCharFormat>
#include <QThread>
#include <QTextLayout>
#include <QTextDocument>
#include <QTimer>

extern "C" {
#include "pmh_parser.h"
}

class WorkerThread : public QThread
{
public:
	~WorkerThread();
	void run();
	char *content;
	pmh_element **result;
};

struct HighlightingStyle
{
	pmh_element_type type;
	QTextCharFormat format;
};


class MarkdownHighlighter : public QObject
{
	Q_OBJECT

public:
	MarkdownHighlighter(QTextDocument *parent = 0, int aWaitInterval = 2000);
	void setStyles(QVector<HighlightingStyle> &styles);
	static bool formatLessThan(const QTextLayout::FormatRange &r1, const QTextLayout::FormatRange &r2);
	int waitInterval;

protected:

private slots:
	void handleContentsChange(int position, int charsRemoved, int charsAdded);
	void threadFinished();
	void timerTimeout();


private:
	QTimer *timer;
	QTextDocument *document;
	WorkerThread *workerThread;
	bool parsePending;
	pmh_element **cached_elements;
	QVector<HighlightingStyle> highlightingStyles;

	void clearFormatting();
	void highlight();
	void parse();
};

#endif
