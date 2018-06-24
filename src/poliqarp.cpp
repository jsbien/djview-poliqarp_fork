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

#include <QtXml>
#include <QtNetwork>
#include "poliqarp.h"
#include "version.h"
#include "messagedialog.h"
#include "replyparser.h"


Poliqarp::Poliqarp(QObject *parent) :
	QObject(parent)
{
	m_network = new QNetworkAccessManager(this);
	connect(m_network, SIGNAL(finished(QNetworkReply*)), this,
			  SLOT(replyFinished(QNetworkReply*)));
	m_matchesFound = 0;
	m_currentSource = 0;
	m_configured = false;
}

void Poliqarp::connectToServer(const QUrl &url)
{
	m_serverUrl = url;
	m_serverUrl.setPath("/");
	m_sources.clear();
	m_replies[ConnectOperation] = m_network->get(request("connect", m_serverUrl));
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
	m_results.clear();
	m_matchesFound = 0;
	QUrl url = m_serverUrl.resolved(currentSource() + "query/");
	QByteArray args("query=");
	args.append(QUrl::toPercentEncoding(text));
	QNetworkRequest r = request("query", url);
	r.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	m_replies[QueryOperation] = m_network->post(r, args);
}

bool Poliqarp::fetchMore()
{
	if (!hasMore())
		return false;
	QString moreMatches = QString("%1+/").arg(m_results.count());
	QUrl url = m_nextQueries.resolved(moreMatches);
	m_replies[QueryOperation] = m_network->get(request("fetch more", url));
	return true;
}

void Poliqarp::fetchMetadata(int index)
{
	if (index < 0 || index >= m_results.count())
		return;
	else if (!m_results[index].metadata().isEmpty())
		emit metadataReceived();
	else {
		if (m_replies.contains(MetadataOperation))
			m_replies[MetadataOperation]->abort();
		QUrl url = m_serverUrl.resolved(currentSource() + QString("query/%1/").arg(index));
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
	if (!m_configured && !m_network->cookieJar()->cookiesForUrl(QUrl(m_serverUrl.scheme() + "://" + m_serverUrl.host())).isEmpty()) {
		m_configured = true;
		updateSettings();
	}

	Operation operation = m_replies.key(reply, InvalidOperation);
	if (reply->error() == QNetworkReply::NoError)
		parseReply(operation, reply);
	else if (reply->error() != QNetworkReply::OperationCanceledError && operation != InvalidOperation)
		MessageDialog::warning(tr("There was a network error:\n%1 for URL:\n%2").arg(reply->errorString()).arg(reply->url().toString()));

	m_replies.remove(m_replies.key(reply));
	reply->deleteLater();
}

void Poliqarp::rerunQuery()
{
	if (m_pendingQuery.isValid())
		m_replies[QueryOperation] = m_network->get(request("rerun query", m_pendingQuery));
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
	ReplyParser parser(reply);
	m_corpusDescription = QString("<h3>%1</h3>").arg(tr("About current corpus"));
	m_corpusDescription.append(parser.textBetweenTags("<div class='corpus-info'>", "</div>"));
	m_corpusDescription.append(parser.textBetweenTags("<div class='corpus-info-suffix'>", "</div>"));
	emit corpusChanged();
}

bool Poliqarp::parseReply(Poliqarp::Operation operation, QNetworkReply *reply)
{
	QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
   if (redirection.isValid() && redirection.scheme().isEmpty()) {
      redirection.setScheme(m_serverUrl.scheme());
      redirection.setHost(m_serverUrl.host());
   }

   if (reply->error() != QNetworkReply::NoError)
		log("error", reply->errorString());
   else if (redirection.isValid())
      log("redirect to", redirection.url());
	else log("received", reply->url());

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
		else {
			selectSourceFinished(reply);
			updateSettings();
		}
		m_replies.remove(QueryOperation);
		m_replies.remove(MetadataOperation);
		break;
	case SettingsOperation:
		if (redirection.isValid())
			m_replies[SettingsOperation] = m_network->get(request("settings", redirection));
		break;
	case QueryOperation:
		if (redirection.isValid()) {
         m_replies[QueryOperation] = m_network->get(request("query", redirection));
		}
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
      break;
	case InvalidOperation:
		break;
	}
	return true;
}

bool Poliqarp::parseSources(QNetworkReply *reply)
{
	m_serverDescription.clear();
	m_corpusDescription.clear();

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

	if (!m_sources.isEmpty()) {
		m_serverDescription = QString("<h3>%1</h3>").arg(tr("About current server"));
		m_serverDescription.append(parser.textBetweenTags("<div class='search-engine-info'>", "</div>"));
	}

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
		m_results.append(item);
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
		emit queryDone(baseMsg.arg(m_results.count()).arg(m_matchesFound));
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
	if (index < 0 || index >= m_results.count())
		return false;

	QDomElement metadata = parser.findElement("div", "class=query-results");
	if (metadata.isNull())
		return false;

	QDomDocument doc;
	QDomElement html = doc.createElement("html");
	html.appendChild(metadata);
	doc.appendChild(html);

	QString server = reply->url().host();
	QDomNodeList links = doc.elementsByTagName("a");
	for (int i = 0; i < links.count(); i++) {
		QDomElement elt = links.at(i).toElement();
		if (elt.hasAttribute("href")) {
			QString href = elt.attribute("href");
			if (href.startsWith("/"))
				elt.setAttribute("href", m_serverUrl.scheme() + "://" + server + href);
		}
	}

	m_results[index].setMetadata(doc.toString());
	return true;
}

void Poliqarp::log(const QString& type, const QUrl& url)
{
	m_logs.addMessage(type, url);
}

QNetworkRequest Poliqarp::request(const QString& type, const QUrl& url)
{
	log(type, url);
	QNetworkRequest r(url);
	QString build;
	if (Version::buildText() != "?")
		build = QString(" (build %1)").arg(Version::buildText());
	r.setRawHeader("User-Agent", QString("DjVuPoliqarp %1%2")
						.arg(Version::versionText()).arg(build).toLatin1());
	return r;
}

DjVuLink Poliqarp::result(int index) const
{
	if (index >= 0 && index < m_results.count())
		return m_results[index];
	else return DjVuLink();
}

QUrl Poliqarp::corpusUrl() const
{
	QUrl corpus = m_serverUrl;
	if (m_currentSource < m_sources.count())
		corpus.setPath(currentSource());
	return corpus;
}

void Poliqarp::updateSettings()
{
	QUrl settingsPage("/settings/");
	QNetworkRequest configure = request("settings", m_serverUrl.resolved(settingsPage));
	configure.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");

	QSettings settings;
	settings.beginGroup(corpusUrl().toString());

	QUrlQuery params;

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

	QUrl url;
	url.setQuery(params);
	QByteArray data = url.toEncoded();

	m_replies[SettingsOperation] = m_network->post(configure, data);
	settings.endGroup();
}

QNetworkReply *Poliqarp::download(const QUrl &url)
{
	return m_network->get(request("external", url));
}

void Poliqarp::clearQuery()
{
	m_results.clear();
	m_matchesFound = 0;
	m_pendingQuery.clear();
	m_replies.remove(QueryOperation);
}







