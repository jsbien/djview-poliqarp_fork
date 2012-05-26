/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef REPLYPARSER_H
#define REPLYPARSER_H

#include <QtXml>

/** The ReplyParser is a class for parsing server output. */

class ReplyParser : public QObject
{
	Q_OBJECT
public:
	explicit ReplyParser(QObject *parent = 0);
	bool parse(QIODevice* reply);
	QString errorMessage() const;
	QDomDocument document() {return m_document;}
signals:

private:
	QDomDocument m_document;
	int m_errorLine;
	int m_errorColumn;
	QString m_errorMessage;
};

#endif // REPLYPARSER_H
