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
#include <QAbstractListModel>
#include <QMap>
#include <QFont>
#include "entrylist.h"

class IndexModel : public QAbstractListModel
{
	Q_OBJECT
public:
	IndexModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	// Entry functions
	bool open(const QString& filename);
	bool save(const QString& filename);
	void clear();
	Entry entry(const QModelIndex& index) const;
	void setEntry(const QModelIndex& index, const Entry& entry);
	bool isModified(const QModelIndex&) const;
	void restoreEntry(const QModelIndex& index);
	void addEntry(const Entry& entry);
	void addEntries(const EntryList& entries);

	// Sorting
	enum SortMethod {SortByIndex, SortAlphabetically, SortByLetters, SortAtergo};
	void setSortingMethod(SortMethod method);

	// Configuration
	void configure();

	enum EntryRoles {EntryCommentRole = Qt::ToolTipRole, EntryLinkRole = Qt::UserRole, EntryDeletedRole, EntrySortRole};

private:
	/** @return sorting key */
	QString sortingKey(const Entry& e) const;
	/** @return a tergo value. */
	QString aTergo(const QString& s) const;

	EntryList m_data;
	QMap<int, Entry> m_undo;
	enum {NormalFont, DeletedFont};
	QList<QFont> m_fonts;
	SortMethod m_sortingMethod;
};

