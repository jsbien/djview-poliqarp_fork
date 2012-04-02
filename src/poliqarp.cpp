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
	 m_matchesFound = 0;
	 m_currentSource = 0;
}

void Poliqarp::connectToServer(const QUrl &url)
{
	 m_serverUrl = url;
	 m_sources.clear();
	 m_lastConnection = m_network->get(QNetworkRequest(url));
	 m_queries.clear();
	 m_matchesFound = 0;
}

void Poliqarp::setCurrentSource(int index)
{
	 if (index < -1 || index >= m_sources.count())
		  index = -1;
	 m_queries.clear();
	 m_matchesFound = 0;
	 m_currentSource = index;
	 if (index != -1) {
		  QUrl url = m_serverUrl.resolved(m_sources[index]);
		  m_lastSource = m_network->get(QNetworkRequest(url));
	 }
}

void Poliqarp::runQuery(const QString &text)
{
	 m_lastQueryText = text;
	 m_queries.clear();
	 m_matchesFound = 0;
	 QUrl url = m_serverUrl.resolved(m_sources[m_currentSource] + "query/");
	 QByteArray args("query=");
	 args.append(QUrl::toPercentEncoding(text));
	 QNetworkRequest request(url);
	 request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	 m_lastQuery = m_network->post(request, args);
}

void Poliqarp::fetchMore()
{
	 if (hasMore()) {
		  QString moreMatches = QString("%1+/").arg(m_queries.count());
		  QUrl url = m_nextQueries.resolved(moreMatches);
		  m_lastQuery = m_network->get(QNetworkRequest(url));
	 }
}

void Poliqarp::replyFinished(QNetworkReply *reply)
{
	 QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

		  if (reply == m_lastConnection) {
		  if (redirection.isValid())
				m_lastConnection = m_network->get(QNetworkRequest(redirection));
		  else connectionFinished(reply);
	 }
	 else if (reply == m_lastSource) {
		  if (redirection.isValid())
				m_lastSource = m_network->get(QNetworkRequest(redirection));
		  else selectSourceFinished(reply);
	 }
	 else if (reply == m_lastQuery) {
		  if (redirection.isValid())
				m_lastQuery = m_network->get(QNetworkRequest(redirection));
		  else queryFinished(reply);
	 }
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
	 parseQuery(reply);
	 emit queryFinished();
}

void Poliqarp::selectSourceFinished(QNetworkReply *reply)
{
	 Q_UNUSED(reply);
	 emit sourceSelected();
}

bool Poliqarp::parseSources(QIODevice* reply)
{
	 QDomDocument document;
	 QString errorMessage;
	 QString body = QString::fromUtf8(reply->readAll());
	 if (!document.setContent(body, false, &errorMessage))
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
	 QString body = QString::fromUtf8(device->readAll());
	 QString errorMessage;
	 int line, column;

	 if (!document.setContent(body, false, &errorMessage, &line, &column)) {
		  qDebug() << errorMessage << "Line: "<< line << "column: "<< column << '\n' << body;
		  return false;
	 }

	 QDomNodeList rows = document.elementsByTagName("tr");
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

	 QDomNodeList paragraphs = document.elementsByTagName("p");
	 for (int i = 0; i < paragraphs.count(); i++)
		  if (m_nextQueries.isEmpty() && paragraphs.at(i).toElement().attribute("class") == "next_page") {
				QDomElement a = paragraphs.at(i).firstChildElement();
				QString next = a.attribute("href");
				next.truncate(next.count() - 4);
				m_nextQueries = m_serverUrl.resolved(next);
		  }

	 QDomNodeList spans = document.elementsByTagName("span");
	 for (int i = 0; i < spans.count(); i++) {
		  QString id = spans.at(i).toElement().attribute("id");
		  if (id.startsWith("matches"))
				m_matchesFound = spans.at(i).toElement().text().toInt();
	 }

	 return m_queries.count();
}

DjVuLink Poliqarp::query(int index) const
{
	 if (index >= 0 && index < m_queries.count())
		  return m_queries[index];
	 else return DjVuLink();
}






