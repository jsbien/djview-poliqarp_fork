/****************************************************************************
*   Copyright (C) 2012-2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#ifndef REPLYPARSER_H
#define REPLYPARSER_H

#include <QtXml>

/** The ReplyParser is a class for parsing server output. */

class ReplyParser
{
	Q_DECLARE_TR_FUNCTIONS(ReplyParser)

public:
	explicit ReplyParser();
	explicit ReplyParser(QIODevice* reply);
	bool parse(QIODevice* reply);
	QString errorMessage() const;
	QString serverOutput() const {return m_content;}
	void saveServerOutput(const QString& filename);
	/** Check if there exists a tag with optional attribute=value requirement. */
	bool containsTag(const QString& tag, const QString& pattern = "") const;
	/** Find content of tag with optional attribute=value requirement. */
	QString tagText(const QString& tag, const QString& pattern = "") const;
	/** Find given element. */
	QDomElement findElement(const QString& tag, const QString& pattern = "") const;
	QDomDocument document() {return m_document;}
	/** @return text between two tags. This uses text, not XML document due to possible
  unclosed tags. */
	QString textBetweenTags(const QString& startTag, const QString &endTag);

private:
	QDomDocument m_document;
	int m_errorLine;
	int m_errorColumn;
	QString m_errorMessage;
	QString m_content;

};


#endif // REPLYPARSER_H
