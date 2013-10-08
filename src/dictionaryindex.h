/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DICTIONARYINDEX_H
#define DICTIONARYINDEX_H

#include <QtCore>

class DictionaryIndex
{
public:
	DictionaryIndex();
	/** Open dictionary. */
	bool open(const QString& dictionary);
	/** Save dictionary. */
	bool save();
	/** Remove all items. */
	void clear();
	/** @return current filename. */
	QString filename() const {return m_filename;}
	/** @return list of items starting with given word. */
	QStringList find(const QString& words) const;
	/** Is dictionary index available. */
	bool isEmpty() const {return m_filename.isEmpty();}
	/** Check if dictionary was modified after reading. */
	bool isModified() const {return m_modified;}
	/** @return url of given word. */
	QUrl url(const QString& word) const;
private:
	void addEntry(const QString& line);

	struct Entry {
		QString word;
		QUrl link;
		QString comment;
		bool isVisible() const {return comment.contains("hidden");}
		void hide() {if (!isVisible()) comment = "hidden";}
		QString toString();
	};

	QVector<Entry> m_entries;
	QString m_filename;
	bool m_modified;
};

#endif // DICTIONARYINDEX_H
