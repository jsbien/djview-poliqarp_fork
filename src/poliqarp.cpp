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
	m_lastQuery = 0;
	m_lastSource = 0;
	m_queryCount = m_currentQuery = 0;
}

void Poliqarp::connectToServer(const QUrl &url)
{
	m_serverUrl = url;
	m_sources.clear();
	m_lastConnection = m_network->get(QNetworkRequest(url));
	m_queries.clear();
	m_queryCount = m_currentQuery = 0;
}

void Poliqarp::setCurrentSource(int index)
{
	if (index < -1 || index >= m_sources.count())
		index = -1;
	m_currentSource = index;
	m_queries.clear();
	m_queryCount = m_currentQuery = 0;
	if (index != -1) {
		QUrl url = m_serverUrl.resolved(m_sources[index]);
		m_lastSource = m_network->get(QNetworkRequest(url));
	}
}

void Poliqarp::query(const QString &text)
{
	m_queries.clear();
	m_queryCount = m_currentQuery = 0;
	QUrl url = m_serverUrl.resolved(m_sources[m_currentSource] + "query/");
	QByteArray args("query=");
	args.append(QUrl::toPercentEncoding(text));
	m_lastQuery = m_network->post(QNetworkRequest(url), args);
}

void Poliqarp::nextQuery()
{
	if (m_currentQuery + QuerySize < m_queries.count()) {
		m_currentQuery += QuerySize;
		emit queryFinished();
	}
	else {
		QString nextMatches = QString("%1+/").arg(m_currentQuery + QuerySize);
		QUrl url = m_nextQueries.resolved(nextMatches);
		m_lastQuery = m_network->get(QNetworkRequest(url));
	}
}

void Poliqarp::previousQuery()
{
	if (m_currentQuery > 0)
		m_currentQuery -= QuerySize;
	emit queryFinished();
}

void Poliqarp::replyFinished(QNetworkReply *reply)
{
	if (reply == m_lastConnection)
		connectionFinished(reply);
	else if (reply == m_lastSource)
		selectSourceFinished(reply);
	else if (reply == m_lastQuery)
		queryFinished(reply);
	reply->deleteLater();
}


void Poliqarp::connectionFinished(QNetworkReply *reply)
{
	if (reply->error())
		emit connectionError(tr("Could not connect to the server.\nPlease check the URL."));
	else if (!parseSources(reply))
		emit connectionError(tr("This does not look like a Poliqarp server."));
}

void Poliqarp::queryFinished(QNetworkReply *reply)
{
	Q_UNUSED(reply);
	parseQuery(reply);
	emit queryFinished();
}

void Poliqarp::selectSourceFinished(QNetworkReply *reply)
{
	Q_UNUSED(reply);
	emit sourceSelected();
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
	QStringList sourceList;
	for (int i = 0; i < sources.count(); i++) {
		QDomElement element = sources.at(i).toElement();
		if (element.isNull())
			continue;
		QString tag = element.attribute("href");
		QString label = element.firstChild().toText().nodeValue();
		if (!tag.isEmpty() && !label.isEmpty()) {
			sourceList.append(label);
			m_sources.append(tag);
		}
	}
	if (m_sources.isEmpty())
		return false;

	emit connected(sourceList);
	return true;
}

bool Poliqarp::parseQuery(QIODevice *device)
{
	QDomDocument document;
	QString d = QString::fromUtf8(device->readAll());
	if (!document.setContent(d, false)) {
		qDebug() << d;
		return false;
	}

	QDomNodeList rows = document.elementsByTagName("tr");
	qDebug() << "Rows";
	for (int i = 0; i < rows.count(); i++) {
		DjVuLink item;
		QDomNodeList fields = rows.at(i).toElement().elementsByTagName("td");
		if (fields.count() != 4)
			continue;
		item.setLeftContext(fields.at(0).toElement().text());
		item.setWord(fields.at(1).toElement().text());
		item.setRightContext(fields.at(2).toElement().text());
		QUrl link = fields.at(3).firstChildElement("a").attribute("href");
		item.setLink(link);
		m_queries.append(item);
	}

	if (m_queryCount && m_queries.count())
		m_currentQuery += QuerySize;

	QDomNodeList paragraphs = document.elementsByTagName("p");
	for (int i = 0; i < paragraphs.count(); i++)
		if (m_nextQueries.isEmpty() && paragraphs.at(i).toElement().attribute("class") == "next_page") {
			QDomElement a = paragraphs.at(i).firstChildElement();
			QString next = a.attribute("href");
			next.truncate(next.count() - 4);
			m_nextQueries = m_serverUrl.resolved(next);
		}
		else if (!m_queryCount && paragraphs.at(i).toElement().text().contains("naleziono")) {
			QString text = paragraphs.at(i).toElement().text();
			int pos = text.indexOf("naleziono");
			m_queryCount = text.mid(pos + 1).section(' ', 1, 1).toInt();
		}
	return m_queries.count();
}

DjVuLink Poliqarp::query(int index) const
{
	index += m_currentQuery;
	if (index >= 0 && index < m_queries.count())
		return m_queries[index];
	else return DjVuLink();
}






