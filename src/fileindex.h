/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef FILEINDEX_H
#define FILEINDEX_H

#include <QtCore>

class FileIndex
{
public:
	struct Entry {
		QString word;
		QUrl link;
		QString comment;
		Entry() {}
		Entry(const QString& w) {word = w.trimmed();}
		bool isVisible() const {return !comment.startsWith('!');}
		bool isValid() const {return !word.isEmpty();}
		void hide() {if (isVisible()) comment.prepend('!');}
		void show() {if (!isVisible()) comment = comment.mid(1);}
		QString formattedWord() const {return link.isValid() ? word : word + ' ';}
		QString toString();
		bool operator==(const Entry& e) {return word == e.word;}
		bool operator<(const Entry& e) {return word < e.word;}
	};


	/** Sort order. */
	enum Flags {OriginalOrder = 0x00, AlphabeticOrder = 0x01, AtergoOrder = 0x02,
				  ViewHidden = 0x04};
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
	QList<Entry> items(int flags = OriginalOrder) const;
	/** Is file index available. */
	bool isEmpty() const {return m_filename.isEmpty();}
	/** Check if file index
	 *was modified after reading. */
	bool isModified() const {return m_modified;}
	/** Hide given word. */
	void hide(const QString& entry);
	/** Show given word. */
	void show(const QString& entry);
	/** @return url of given word. */
	QUrl link(const QString& word) const;
	/** @return validated url of given word or empty URL. This ensures
	URL uses http, https or ftp protocol. */
	QUrl validLink(const QString& word) const;
	/** Set link for given word. */
	bool setLink(const QString& word, const QUrl& link);
	/** @return comment for given word. */
	QString comment(const QString &word) const;
	/** Set comment for given word. */
	bool setComment(const QString& word, const QString& comment);
	/** Add new entry . */
	bool addEntry(const Entry& entry);
	/** @return entry for given word. */
	Entry entry(const QString& word) const;
	/** Update entry for given word. */
	void setEntry(const QString& word, const FileIndex::Entry& entry);
private:
	Entry parseEntry(const QString& line) const;
	/** Reverse all strings for a tergo sorting. */
	QString reverseString(const QString& s) const;

	// CSV
	/** CSV row to string list. */
	static QStringList csvToStringList(const QString& row);
	/** String list to CSV row. */
	static QString stringListToCsv(const QStringList& columns);


	struct AlphabeticComparator {
		AlphabeticComparator() {}
		bool operator()(const Entry& e1, const Entry& e2) const
			{return QString::localeAwareCompare(e1.word, e2.word) < 0;}
	};

	QList<Entry> m_entries;
	QString m_filename;
	bool m_modified;
	static QSet<QString> m_backedUp;
};

#endif // FILEINDEX_H
