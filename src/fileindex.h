/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef FILEINDEX_H
#define FILEINDEX_H

#include "entry.h"

class FileIndex
{
public:
	/** Sort order. */
	enum SortOrder {OriginalOrder, AlphabeticOrder, LetterOrder, AtergoOrder};
	FileIndex();
	/** Open index. */
	bool open(const QString& filename);
	/** Save index file. */
	bool save();
	/** Remove all items. */
	void clear();
	/** @return current filename. */
	QString filename() const {return m_filename;}
	/** @return list of items. */
	void sort(SortOrder order = OriginalOrder);
	/** Is file index available. */
	bool isEmpty() const {return m_filename.isEmpty();}
	/** Check if file index
	 *was modified after reading. */
	bool isModified() const {return m_modified;}
	/** Hide given word. */
	void hide(int index);
	/** Show given word. */
	void show(int index);
	/** @return url of given word. */
	QUrl link(int index) const;
	/** Set link for given word. */
	void setLink(int index, const QUrl& link);
	/** @return comment for given word. */
	QString comment(int index) const;
	/** Set comment for given word. */
	void setComment(int index, const QString& comment);
	/** Add new entry . */
	bool appendEntry(const Entry& entry);
	/** @return entry at given position. */
	Entry entry(int index) const;
	/** Update entry for given word. */
	void setEntry(int index, const Entry& entry);
	/** @return number of visible entries. */
	int count() const {return m_sortOrder.count();}
	/** Show hidden items. */
	void showHidden(bool enabled) {m_showHidden = enabled;}
private:

	struct AlphabeticComparator {
		AlphabeticComparator(const QList<Entry>& entries) : m_entries(entries) {}
		bool operator()(int e1, int e2) const
			{return QString::localeAwareCompare(m_entries[e1].word, m_entries[e2].word) < 0;}
		const QList<Entry>& m_entries;
	};

	struct AtergoComparator {
		AtergoComparator(const QList<Entry>& entries) : m_entries(entries) {}
		QString atergo(const QString& s) const;
		bool operator()(int e1, int e2) const
			{return QString::localeAwareCompare(atergo(m_entries[e1].word),
															atergo(m_entries[e2].word)) < 0;}
		const QList<Entry>& m_entries;
	};

	struct LetterComparator {
		LetterComparator(const QList<Entry>& entries) : m_entries(entries) {}
		QString letters(const QString& s) const;
		bool operator()(int e1, int e2) const
			{return QString::localeAwareCompare(letters(m_entries[e1].word),
															letters(m_entries[e2].word)) < 0;}
		const QList<Entry>& m_entries;
	};


	QList<Entry> m_entries;
	QList<int> m_sortOrder;
	QString m_filename;
	bool m_modified;
	bool m_showHidden;
	static QSet<QString> m_backedUp;
};

#endif // FILEINDEX_H
