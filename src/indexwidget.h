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
	void addEntry(const FileIndex::Entry& entry);
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
	/** Show or edit entry based on Ctrl modifier. */
	void activateEntry();
	/** Go to matching index. */
	void findEntry();
	/** Find next or show document. */
	void entryTriggered();
	/** Entry was activated. */
	void showCurrent();
	/** Edit current entry. */
	void editEntry();
	/** Mark current entry as hidden and remove it from the list. */
	void hideCurrent();
	/** Mark current entry as visible. This is accessible only if hidden items are visible. */
	void unhideCurrent();
	/** Set list content to current file index. */
	void updateList();
	/** Update actions depending on current item. */
	void updateActions();

private:
	/** Close current corpus, saving if necessary. */
	void close();
	/** Do search for text. */
	void doSearch(int start, const QString& text);
	/** Create or update item for given entry. */
	void updateItem(QListWidgetItem* item, const FileIndex::Entry& entry) const;

	Ui::IndexWidget ui;
	FileIndex m_fileIndex;
	QActionGroup* m_sortGroup;
	QIcon m_commentIcon;
};

#endif // INDEXWIDGET_H
