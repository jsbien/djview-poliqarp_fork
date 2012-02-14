/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtXml>
#include "poliqarp.h"

Poliqarp::Poliqarp(QObject *parent) :
	 QObject(parent)
{
	m_network = new QNetworkAccessManager(this);
	connect(m_network, SIGNAL(finished(QNetworkReply*)), this,
			  SLOT(replyFinished(QNetworkReply*)));

	m_lastConnection = 0;
}

void Poliqarp::connectToServer(const QUrl &url)
{
	m_sources.clear();
	m_lastConnection = m_network->get(QNetworkRequest(url));
}

void Poliqarp::replyFinished(QNetworkReply *reply)
{
	if (reply->error())
		emit connectionError(tr("Could not connect to the server.\nPlease check the URL."));
	else if (!parseSources(reply))
		emit connectionError(tr("This does not look like a Poliqarp server."));
	else emit connected();
}

bool Poliqarp::parseSources(QIODevice* device)
{
	QDomDocument document;
	if (!document.setContent(device, false))
		return false;

	QDomElement title = document.elementsByTagName("title").at(0).toElement();
	if (!title.firstChild().toText().nodeValue().contains("Poliqarp"))
		return false;

	QDomElement list = document.elementsByTagName("ul").at(1).toElement();
	if (list.isNull())
		return false;

	QDomNodeList sources = list.elementsByTagName("a");
	for (int i = 0; i < sources.count(); i++) {
		QDomElement element = sources.at(i).toElement();
		if (element.isNull())
			continue;
		QString tag = element.attribute("href");
		QString label = element.firstChild().toText().nodeValue();
		if (!tag.isEmpty() && !label.isEmpty())
			m_sources.append(label + "=" + tag);
	}
	return m_sources.count() > 0;
}


