/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#pragma once
#include "ui_indexwidget.h"
#include "djvulink.h"
#include "history.h"

class Entry;
class IndexModel;

class IndexWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IndexWidget(QWidget *parent = 0);
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


	// Update entries
	/** Add new entry to the end of the index. */
	void addEntry(const Entry& entry);
	/** Update region for current index entry. */
	void updateEntry(const QUrl &link);



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
	// Update entries
	/** Show current entry. */
	void activateEntry(const QModelIndex& index = QModelIndex());
	/** Add entry to history. */
	void addToHistory(const QModelIndex& previous, const QModelIndex& current);
	/** Menu requested. */
	void menuRequested(const QPoint& point);
	/** Edit current entry. */
	void editEntry();
	/** Delete/undelete current entry. */
	void deleteEntry();

	/** Update sorting. */
	void sort();
	/** Go to matching index. */
	void findEntry();

	/** Update index title consisting of filename and modification flag. */
	void setModified(bool enabled);

	Ui::IndexWidget ui;
	History<QModelIndex> m_history;
	QActionGroup* m_sortGroup;
	QIcon m_commentIcon;
	IndexModel* m_model;
	QSortFilterProxyModel* m_sortModel;

	bool m_modified;
	QString m_filename;
};
