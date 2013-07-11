/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtXml>
#include "poliqarp.h"
#include "version.h"
#include "messagedialog.h"
#include "replyparser.h"

class MyCookieJar : public QNetworkCookieJar
{
public:
	MyCookieJar(QObject* parent) : QNetworkCookieJar(parent) {}
	QList<QNetworkCookie> getAllCookies() { return allCookies(); }
};


Poliqarp::Poliqarp(QObject *parent) :
	QObject(parent)
{
	m_network = new QNetworkAccessManager(this);
	m_network->setCookieJar(new MyCookieJar(this));
	connect(m_network, SIGNAL(finished(QNetworkReply*)), this,
			  SLOT(replyFinished(QNetworkReply*)));
	m_matchesFound = 0;
	m_currentSource = 0;
	m_configured = false;
}

void Poliqarp::connectToServer(const QUrl &url)
{
	m_serverUrl = url;
	m_sources.clear();
	m_replies[ConnectOperation] = m_network->get(request("connect", url));
	clearQuery();
}

void Poliqarp::setCurrentSource(int index)
{
	if (index < -1 || index >= m_sources.count())
		index = -1;
	m_currentSource = index;
	clearQuery();
	if (index != -1) {
		QUrl url = m_serverUrl.resolved(m_sources[index]);
		m_replies[SourceOperation] = m_network->get(request("sources", url));
	}
}

void Poliqarp::runQuery(const QString &text)
{
	m_pendingQuery.clear();
	m_queries.clear();
	m_matchesFound = 0;
	QUrl url = m_serverUrl.resolved(m_sources[m_currentSource] + "query/");
	QByteArray args("query=");
	args.append(QUrl::toPercentEncoding(text));
	QNetworkRequest r = request("query", url);
	r.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	m_replies[QueryOperation] = m_network->post(r, args);
}

bool Poliqarp::fetchMore()
{
	if (!hasMore())
		return false;
	QString moreMatches = QString("%1+/").arg(m_queries.count());
	QUrl url = m_nextQueries.resolved(moreMatches);
	m_replies[QueryOperation] = m_network->get(request("fetch more", url));
	return true;
}

void Poliqarp::fetchMetadata(int index)
{
	if (index < 0 || index >= m_queries.count())
		return;
	else if (!m_queries[index].metadata().isEmpty())
		emit metadataReceived();
	else {
		if (m_replies.contains(MetadataOperation))
			m_replies[MetadataOperation]->abort();
		QUrl url = m_serverUrl.resolved(m_sources[m_currentSource] + QString("query/%1/").arg(index));
		m_replies[MetadataOperation] = m_network->get(request("metatada", url));
	}
}

void Poliqarp::abortQuery()
{
	if (m_replies[QueryOperation])
		m_replies[QueryOperation]->abort();

}

void Poliqarp::replyFinished(QNetworkReply *reply)
{
	if (!m_configured && !m_network->cookieJar()->cookiesForUrl(QUrl("http://" + m_serverUrl.host())).isEmpty()) {
		m_configured = true;
		updateSettings();
	}

	Operation operation = m_replies.key(reply, InvalidOperation);
	if (reply->error() == QNetworkReply::NoError)
		parseReply(operation, reply);
	else if (reply->error() != QNetworkReply::OperationCanceledError && operation != InvalidOperation)
		MessageDialog::warning(tr("There was a network error:\n%1").arg(reply->errorString()));

	m_replies.remove(m_replies.key(reply));
	reply->deleteLater();
}

void Poliqarp::rerunQuery()
{
	if (m_pendingQuery.isValid())
		m_replies[QueryOperation] = m_network->get(request("query", m_pendingQuery));
}


void Poliqarp::connectionFinished(QNetworkReply *reply)
{
	if (reply->error())
		emit serverError(tr("Could not connect to the server.\nPlease check the URL."));
	else if (!parseSources(reply))
		emit serverError(tr("This does not look like a Poliqarp server."));
}

void Poliqarp::selectSourceFinished(QNetworkReply *reply)
{
	QString body = QString::fromUtf8(reply->readAll());
	QString info = textBetweenTags(body, "<div class='corpus-info'>", "</div>");
	info.append(textBetweenTags(body, "<div class='corpus-info-suffix'>", "</div>"));
	emit sourceSelected(info);
}

bool Poliqarp::parseReply(Poliqarp::Operation operation, QNetworkReply *reply)
{
	QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	switch (operation) {
	case ConnectOperation:
		if (redirection.isValid())
			m_replies[ConnectOperation] = m_network->get(request("connect", redirection));
		else connectionFinished(reply);
		m_replies.remove(QueryOperation);
		m_replies.remove(MetadataOperation);
		break;
	case SourceOperation:
		if (redirection.isValid())
			m_replies[SourceOperation] = m_network->get(request("sources", redirection));
		else selectSourceFinished(reply);
		m_replies.remove(QueryOperation);
		m_replies.remove(MetadataOperation);
		break;
	case SettingsOperation:
		if (redirection.isValid())
			m_replies[SettingsOperation] = m_network->get(request("settings", redirection));
		break;
	case QueryOperation:
		if (redirection.isValid())
			m_replies[QueryOperation] = m_network->get(request("query", redirection));
		else if (!parseQuery(reply)) {
			QString refresh = QString::fromUtf8(reply->rawHeader("Refresh"));
			if (!refresh.isEmpty()) {
				int msec = refresh.section(';', 0, 0).toInt();
				m_pendingQuery = m_serverUrl.resolved(refresh.section('=', 1));
				if (msec > 0 && m_pendingQuery.isValid())
					QTimer::singleShot(qMin(1000, msec), this, SLOT(rerunQuery()));
			}
		}
		break;
	case MetadataOperation:
		if (redirection.isValid())
			m_replies[MetadataOperation] = m_network->get(request("metadata", redirection));
		else {
			if (parseMetadata(reply))
				emit metadataReceived();
		}
	case InvalidOperation:
		break;
	}
	return true;
}

bool Poliqarp::parseSources(QNetworkReply *reply)
{
	ReplyParser parser;
	if (!parser.parse(reply)) {
		MessageDialog::warning(tr("Error reading list of corpora: %1")
									  .arg(parser.errorMessage()));
		return false;
	}

	if (!parser.tagText("title").contains("Poliqarp"))
		return false;

	QDomElement list = parser.document().elementsByTagName("ul").at(1).toElement();
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
		if (!tag.isEmpty() && !label.isEmpty() && !sourceList.contains(label)) {
			sourceList.append(label);
			m_sources.append(tag);
		}
	}
	if (m_sources.isEmpty())
		return false;

	emit connected(sourceList);
	return true;
}

bool Poliqarp::parseQuery(QNetworkReply *reply)
{
	ReplyParser parser;
	if (!parser.parse(reply)) {
		if (MessageDialog::yesNoQuestion(parser.errorMessage() + "\n" +
												  tr("Do you want to save HTML file sent by server?"))) {
			QString filename = MessageDialog::saveFile(tr("HTML files (*.html)"));
			if (!filename.isEmpty())
				parser.saveServerOutput(filename);
		}
		return false;
	}

	if (parser.containsTag("span", "id=wait"))
		return false;
	else if (parser.containsTag("ul", "class=errorlist")) {
		emit serverError(parser.findElement("ul", "class=errorlist").firstChild().toElement().text());
		return false;
	}


	QDomNodeList rows = parser.document().elementsByTagName("tr");
	for (int i = 0; i < rows.count(); i++) {
		DjVuLink item;
		QDomNodeList fields = rows.at(i).toElement().elementsByTagName("td");
		if (fields.count() < 4 || fields.count() > 5)
			continue;
		item.setLeftContext(fields.at(0).toElement().text());
		item.setMatch(fields.at(1).toElement().text());
		if (fields.count() == 4) {
			item.setRightContext(fields.at(2).toElement().text());
			item.setLink(fields.at(3).firstChildElement("a").attribute("href"));
		}
		else {
			item.setRightMatch(fields.at(2).toElement().text());
			item.setRightContext(fields.at(3).toElement().text());
			item.setLink(fields.at(4).firstChildElement("a").attribute("href"));
		}
		m_queries.append(item);
	}

	QDomElement next = parser.findElement("p", "class=next_page");
	if (!next.isNull()) {
		QString nextQueries = next.firstChildElement().attribute("href");
		nextQueries.truncate(nextQueries.count() - 4);
		m_nextQueries = m_serverUrl.resolved(nextQueries);
	}

	bool soFar = parser.containsTag("span", "id=matches-so-far");
	if (soFar)
		m_matchesFound = parser.tagText("span", "id=matches-so-far").toInt();
	else m_matchesFound = parser.tagText("span", "id=matches").toInt();

	if (m_matchesFound == 0)
		emit queryDone(tr("No matches found"));
	else {
		QString baseMsg = soFar ? tr("Matches so far: %1 of %2") : tr("Matches %1 of %2");
		emit queryDone(baseMsg.arg(m_queries.count()).arg(m_matchesFound));
	}
	return true;
}

bool Poliqarp::parseMetadata(QNetworkReply *reply)
{
	ReplyParser parser;
	if (!parser.parse(reply)) {
		MessageDialog::warning(tr("Error reading metadata: %1")
									  .arg(parser.errorMessage()));
		return false;
	}

	int index = reply->url().toString().section('/', -2, -2).toInt();
	if (index < 0 || index >= m_queries.count())
		return false;

	QDomElement metadata = parser.findElement("div", "class=query-results");
	if (metadata.isNull())
		return false;

	QDomDocument doc;
	QDomElement html = doc.createElement("html");
	html.appendChild(metadata);
	doc.appendChild(html);
	m_queries[index].setMetadata(doc.toString());
	return true;
}

QNetworkRequest Poliqarp::request(const QString& type, const QUrl& url)
{
	if (m_logs.count() > 100)
		m_logs.removeFirst();
	m_logs.append(QString("%1: %2 %3")
					  .arg(QTime::currentTime().toString())
					  .arg(type).arg(url.toString()));

	QNetworkRequest r(url);
	r.setRawHeader("User-Agent", QString("DjVuPoliqarp %1 (build %2)")
						.arg(Version::versionText())
						.arg(Version::buildText()).toAscii());
	return r;
}

QString Poliqarp::textBetweenTags(const QString& body, const QString &startTag,
											 const QString &endTag)
{
	int start = body.indexOf(startTag);
	int end =  body.indexOf(endTag, start + startTag.count() + 1);
	if (start != -1 && end != -1)
		return body.mid(start + startTag.count(),
							 end - start - startTag.count());
	else return QString();
}

DjVuLink Poliqarp::query(int index) const
{
	if (index >= 0 && index < m_queries.count())
		return m_queries[index];
	else return DjVuLink();
}

void Poliqarp::updateSettings()
{
	QUrl settingsPage("/en/settings/");
	QNetworkRequest configure = request("settings", m_serverUrl.resolved(settingsPage));
	configure.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

	QSettings settings;
	settings.beginGroup(m_serverUrl.host());

	QUrl params;
	params.addQueryItem("random_sample", settings.value("random_sample", 0).toString());
	params.addQueryItem("random_sample_size",  settings.value("random_sample_size", 50).toString());
	params.addQueryItem("sort", settings.value("sort", 0).toString());
	params.addQueryItem("sort_column", settings.value("sort_column", "lc").toString()); // lc, lm, rm, rc
	params.addQueryItem("sort_type", settings.value("sort_type", "afronte").toString()); // afronte, atergo
	params.addQueryItem("sort_direction", settings.value("sort_direction", "asc").toString());  // asc, desc
	QString showMatch = settings.value("show_in_match", "s").toString();   // s, l, t
	for (int i = 0; i < showMatch.count(); i++)
		params.addQueryItem("show_in_match", QString(showMatch[i]));
	QString showContext = settings.value("show_in_context", "s").toString();   // s, l, t
	for (int i = 0; i < showContext.count(); i++)
		params.addQueryItem("show_in_context", QString(showContext[i]));
	params.addQueryItem("left_context_width", settings.value("left_context_width", 5).toString());  // 5
	params.addQueryItem("right_context_width", settings.value("right_context_width", 5).toString());  // 5
	params.addQueryItem("wide_context_width", settings.value("wide_context_width", 50).toString());  // 50
	params.addQueryItem("graphical_concordances", "0"); // 50
	params.addQueryItem("results_per_page", "25"); // 25
	QByteArray data = params.encodedQuery();

	m_replies[SettingsOperation] = m_network->post(configure, data);
	settings.endGroup();
}

void Poliqarp::clearQuery()
{
	m_queries.clear();
	m_matchesFound = 0;
	m_pendingQuery.clear();
	m_replies.remove(QueryOperation);
}







