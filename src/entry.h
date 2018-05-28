/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf
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
#include <QtCore>

class Entry {
public:
	Entry() {m_deleted = false;}
	Entry(const QString& w) {m_word = w.trimmed(); m_deleted = false;}

	// Getters and setters
	QString word() const;
	void setWord(const QString& word);
	QUrl link() const;
	void setLink(const QUrl& link);
	QString comment() const;
	void setComment(const QString& comment);
	QString description() const;
	void setDescription(const QString& description);
	void setDeleted(bool deleted);
	bool isDeleted() const;

	// Query
	bool isValid() const {return !m_word.isEmpty();}
	QUrl validLink() const;
	QString toString() const;
	/** Combination of word and description. */
	QString title() const;

	// Comparison
	bool operator==(const Entry& e) {return m_word == e.m_word;}
	bool operator<(const Entry& e) {return m_word < e.m_word;}

	// Is Entry exact copy of another entry? This is used to recognize
	bool isCopyOf(const Entry& e) const;


	/** Parse string. */
	static Entry parse(const QString& line);
	/** CSV row to string list. */
	static QStringList csvToStringList(const QString& row);
	/** String list to CSV row. */
	static QString stringListToCsv(const QStringList& columns);
private:
	QString m_word;
	QUrl m_link;
	QString m_comment;
	QString m_description;
	bool m_deleted;
};

