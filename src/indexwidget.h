/****************************************************************************
*   This software is subject to, and may be distributed under, the
*   GNU General Public License, either version 2 of the license,
*   or (at your option) any later version. The license should have
*   accompanied the software or you may obtain a copy of the license
*   from the Free Software Foundation at http://www.fsf.org .
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
****************************************************************************/

#pragma once
#include "ui_indexwidget.h"
#include "history.h"
#include "replacements.h"

class DjVuLink;
class Entry;
class IndexModel;
class QSortFilterProxyModel;

class IndexWidget : public QWidget
{
	Q_OBJECT
public:
	explicit IndexWidget(QWidget *parent = nullptr);
	~IndexWidget();

	// IO
	/** Read index from file. Ask to save previous first */
	bool open(const QString& filename);
   /** Create new index. */
   void create();
	/** Save file. */
	void save();
   /** Save file as. */
   void saveAs();
	/** Close file and remove data. */
	void close();
	/** Close current index, saving if necessary. */
	bool queryClose();
	/** Reload file */
	void reload();
	/** @return current filename. */
	QString filename() const;

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
	/** Index was open */
	void opened(const QString& filename);
	/** Entry was double-clicked. */
	void documentRequested(const DjVuLink& link);
	/** Index was saved. */
	void saved(const QString& message, int timeout);
	/** History has changed. */
	void historyChanged(const QString& previous, const QString& next);
   /** Add to graphical results. */
   void addToResults(const DjVuLink& link);
private:
   /** @return current index entry. */
   DjVuLink currentDjVu() const;
   /** Add to graphical results. */
   void bookmark();
	/** @return current index. */
	QModelIndex currentEntry() const;
	/** Add entry to history. */
	void currentIndexChanged(const QModelIndex& previous, const QModelIndex& current = QModelIndex());
	/** Menu requested. */
	void menuRequested(const QPoint& point);
	/** Edit current entry. */
	void editEntry();
	/** Reread entry from file. */
	void restoreEntry();
	/** Delete/undelete current entry. */
	void deleteEntry();
	/** Edit current comment. */
	void editComment();
   /** Copy entry name. */
   void copy();
   /** Copy entry description. */
   void copyDescription();

	/** Update sorting. */
	void sort();
	/** Go to matching index. */
	void findEntry();
	/** Go to next matching index. */
	void findNextEntry();
	/** Find starting from given index. */
	void findEntryFrom(const QModelIndex& index);

	/** Update index title consisting of filename and modification flag. */
	void setModified(bool enabled);

	Ui::IndexWidget ui;
	History<QModelIndex> m_history;
	QActionGroup* m_sortGroup;
	QIcon m_commentIcon;
	IndexModel* m_model;
	QSortFilterProxyModel* m_sortModel;
	Replacements m_urlReplacements;

	bool m_modified;
	QString m_filename;
};
