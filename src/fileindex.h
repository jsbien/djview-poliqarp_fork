/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef FILEINDEX_H
#define FILEINDEX_H

#include <QtCore>

class FileIndex
{
public:
	/** Sort order. */
	enum SortOrder {OriginalOrder, AlphabeticOrder, AtergoOrder};
	FileIndex();
	/** Open index. */
	bool open(const QString& filename);
	/** Save dictionary. */
	bool save();
	/** Remove all items. */
	void clear();
	/** @return current filename. */
	QString filename() const {return m_filename;}
	/** @return list of items. */
	QStringList items(SortOrder order = OriginalOrder) const;
	/** Is dictionary index available. */
	bool isEmpty() const {return m_filename.isEmpty();}
	/** Check if dictionary was modified after reading. */
	bool isModified() const {return m_modified;}
	/** Hide given word. */
	void hide(const QString& entry);
	/** @return url of given word. */
	QUrl link(const QString& word) const;
	/** Set link for given word. */
	bool setLink(const QString& word, const QUrl& link);
	/** @return comment for given word. */
	QString comment(const QString &word) const;
	/** Set comment for given word. */
	bool setComment(const QString& word, const QString& comment);
	/** Add new word. */
	bool addWord(const QString&, const QUrl& link);
private:
	void addEntry(const QString& line);

	struct Entry {
		QString word;
		QUrl link;
		QString comment;
		Entry() {}
		Entry(const QString& w) {word = w.trimmed();}
		bool isVisible() const {return !comment.startsWith('!');}
		void hide() {if (isVisible()) comment.prepend('!');}
		QString formattedWord() const {return link.isValid() ? word : word + ' ';}
		QString toString();
		bool operator==(const Entry& e) {return word == e.word;}
		bool operator<(const Entry& e) {return word < e.word;}
	};

	struct AtergoComparator {
		AtergoComparator() {}
		bool operator()(const Entry& e1, const Entry& e2);
	};

	struct AlphabeticComparator {
		AlphabeticComparator() {}
		bool operator()(const Entry& e1, const Entry& e2)
			{return e1.word.compare(e2.word, Qt::CaseInsensitive) < 0;}
	};

	QVector<Entry> m_entries;
	QString m_filename;
	bool m_modified;
};

#endif // FILEINDEX_H
