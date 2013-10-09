/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "ui_indexwidget.h"
#include "fileindex.h"
#include "djvulink.h"

class IndexWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IndexWidget(QWidget *parent = 0);
	/** Save document if modified. */
	~IndexWidget();
	/** Read index for given corpus. Previous will be automatically saved if modified. */
	void open(const QString& corpus);
public slots:
	/** Add new entry to the end of the index. */
	void addEntry(const QString &word, const QUrl &link);
	/** Update region for current index entry. */
	void updateCurrentEntry(const QUrl &link);
signals:
	/** File index is available for current corpus. */
	void indexOpened();
	/** File index is not available for current corpus. */
	void indexClosed();
	/** Entry was double-clicked. */
	void documentRequested(const DjVuLink& link);
private slots:
	/** Go to matching index. */
	void findEntry();
	/** Go to next matching index. */
	void findNextEntry();
	/** Entry was activated. */
	void showCurrent();
	/** Mark current entry as hidden and remove it from the list. */
	void hideCurrent();
private:
	/** Close current corpus, saving if necessary. */
	void close();
	/** Set list content to current file index. */
	void updateList();
	/** Do search for text. */
	void doSearch(int start, const QString& text);

	Ui::IndexWidget ui;
	FileIndex m_fileIndex;
};

#endif // INDEXWIDGET_H
