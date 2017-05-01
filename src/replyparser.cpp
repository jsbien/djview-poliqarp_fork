/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#include "replyparser.h"

ReplyParser::ReplyParser()
{
}

ReplyParser::ReplyParser(QIODevice* reply)
{
	parse(reply);
}

bool ReplyParser::parse(QIODevice *reply)
{
	m_document.clear();
	m_errorLine = m_errorColumn = 0;
	m_errorMessage.clear();

	m_content = QString::fromUtf8(reply->readAll());
	if (m_content.contains("<span>(404)</span>")) {
		m_errorMessage = tr("Page not found");
		return false;
	}
	else if (m_content.contains("<span>(500)</span>")) {
		m_errorMessage = tr("Internal server error");
		return false;
	}
	else return m_document.setContent(m_content, false, &m_errorMessage,
										  &m_errorLine, &m_errorColumn);
}

QString ReplyParser::errorMessage() const
{
	if (m_errorLine > 0)
		return tr("Error in line %1, column %2:\n%3")
			.arg(m_errorLine).arg(m_errorColumn).arg(m_errorMessage);
	else return m_errorMessage;
}

void ReplyParser::saveServerOutput(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		stream << m_content;
	}
}

bool ReplyParser::containsTag(const QString &tag, const QString& pattern) const
{
	return !findElement(tag, pattern).isNull();
}

QString ReplyParser::tagText(const QString &tag, const QString& pattern) const
{
	QDomElement elt = findElement(tag, pattern);
	return elt.isNull() ? "" : elt.text();
}

QDomElement ReplyParser::findElement(const QString &tag, const QString &pattern) const
{
	QString attribute = pattern.section('=', 0, 0);
	QString value = pattern.section('=', 1, 1);

	QDomNodeList tags = m_document.elementsByTagName(tag);
	for (int i = 0; i < tags.count(); i++) {
		QDomElement elt = tags.at(i).toElement();
		if (attribute.isEmpty() || elt.attribute(attribute) == value)
			return elt;
	}
	return QDomElement();
}

QString ReplyParser::textBetweenTags(const QString &startTag, const QString &endTag)
{
	int start = m_content.indexOf(startTag);
	int end =  m_content.indexOf(endTag, start + startTag.count() + 1);
	if (start != -1 && end != -1)
		return m_content.mid(start + startTag.count(),
							 end - start - startTag.count());
	else return QString();

}
