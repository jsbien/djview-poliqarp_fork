/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "replyparser.h"

ReplyParser::ReplyParser(QObject *parent) :
	QObject(parent)
{
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
