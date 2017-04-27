/****************************************************************************
*   Copyright (C) 2014 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#pragma once
#include <QtCore>

class Entry {
public:
	Entry() {m_deleted = false;}
	Entry(const QString& w) {word = w.trimmed(); m_deleted = false;}
	bool isValid() const {return !word.isEmpty();}
	void setDeleted(bool deleted);
	bool isDeleted() const;
	QString formattedWord() const {return link.isValid() ? word : word + ' ';}
	QString toString() const;
	bool operator==(const Entry& e) {return word == e.word;}
	bool operator<(const Entry& e) {return word < e.word;}
	QUrl validLink() const;

	QString word;
	QUrl link;
	QString comment;

	/** Parse string. */
	static Entry parse(const QString& line);
	/** CSV row to string list. */
	static QStringList csvToStringList(const QString& row);
	/** String list to CSV row. */
	static QString stringListToCsv(const QStringList& columns);
private:

	bool m_deleted;
};

