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

	QString body = QString::fromUtf8(reply->readAll());
	if (!m_document.setContent(body, false, &m_errorMessage, &m_errorLine, &m_errorColumn))
		return false;

	return true;
}

QString ReplyParser::errorMessage() const
{
	return tr("Error parsing server output (line %1, column %2)\nServer output was:\n\n%3\n")
			.arg(m_errorLine).arg(m_errorColumn).arg(m_errorMessage);
}
