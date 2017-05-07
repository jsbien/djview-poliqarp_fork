/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#pragma once
#include <QAbstractListModel>
#include <QFont>
#include "entrylist.h"

class IndexModel : public QAbstractListModel
{
	Q_OBJECT
public:
	IndexModel(QObject* parent = 0);
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	// Entry functions
	bool open(const QString& filename);
	bool save(const QString& filename);
	void clear();
	Entry entry(const QModelIndex& index) const;
	void setEntry(const QModelIndex& index, const Entry& entry);
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
	enum {NormalFont, DeletedFont};
	QList<QFont> m_fonts;
	SortMethod m_sortingMethod;
};

