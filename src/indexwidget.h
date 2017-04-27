/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "ui_indexwidget.h"
#include "fileindex.h"
#include "djvulink.h"
#include "history.h"

class IndexModel;

class IndexWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IndexWidget(QWidget *parent = 0);
	/** Save document if modified. */
	~IndexWidget();

	// IO
	/** Read index from file. Ask to save previous first */
	bool open(const QString& filename);
	/** Save file. */
	void save();
	/** Close file and remove data. */
	void close();
	/** Close current index, saving if necessary. */
	bool queryClose();
	/** Reload file */
	void reload();



	/** Add new entry to the end of the index. */
	void addEntry(const Entry& entry);
	/** Update region for current index entry. */
	void updateCurrentEntry(const QUrl &link);
	/** Configure font. */
	void configure();
	/** Forward. */
	void showNextEntry();
	/** Backward. */
	void showPreviousEntry();
	/** Append index file. */
	void append();
signals:
	/** Entry was double-clicked. */
	void documentRequested(const DjVuLink& link);
	/** Index was saved. */
	void saved(const QString& message, int timeout);
	/** History has changed. */
	void historyChanged(const QString& previous, const QString& next);
private:
	// Display
	/** Show current entry. */
	void activateEntry(const QModelIndex& index = QModelIndex());
	/** Menu requested. */
	void menuRequested(const QPoint& point);

	/** Go to matching index. */
	void findEntry();
	/** Find next or show document. */
	void entryTriggered();

	// Modify
	/** Edit current entry. */
	void editEntry();
	/** Delete/undelete current entry. */
	void deleteEntry();
	/** Set list content to current file index. */
	void sort();
	/** Update actions depending on current item. */
	void indexChanged(int row);
	/** Update index title consisting of filename and modification flag. */
	void setModified(bool enabled);

private:
	/** Do search for text. */
	void doSearch(int start, const QString& text);

	Ui::IndexWidget ui;
	FileIndex m_fileIndex;
//	History<QListWidgetItem*> m_history;
	QActionGroup* m_sortGroup;
	QIcon m_commentIcon;
	IndexModel* m_model;

	bool m_modified;
	QString m_filename;
};

#endif // INDEXWIDGET_H
