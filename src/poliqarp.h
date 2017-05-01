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

#ifndef POLIQARP_H
#define POLIQARP_H

#include <QtNetwork>
#include "djvulink.h"
#include "log.h"

class Poliqarp : public QObject
{
	  Q_OBJECT
private:
	 enum {QuerySize = 25};
public:
	 explicit Poliqarp(QObject *parent = 0);
	 void log(const QString& message, const QUrl& url);
	 DjVuLink result(int index)	const;
	 QUrl serverUrl() const	{return m_serverUrl;}
	 QUrl corpusUrl() const;
	 QString currentSource() const {return m_sources.value(m_currentSource);}
	 QStringList logs() const {return m_logs.items();}
	 void clearLog() {m_logs.clear();}
	 /** Send current configuration to the server. */
	 void updateSettings();
	 /** @return server description of current server in HTML format. */
	 QString serverDescription() const {return m_serverDescription;}
	 /** @return description of current corpus in HTML format. */
	 QString corpusDescription() const {return m_corpusDescription;}
	 int queryCount() const	{return m_results.count();}
	 int matchesFound() const {return m_matchesFound;}
	 bool hasMore() const	{return m_results.count() < m_matchesFound;}

public slots:
	 void connectToServer(const QUrl& url);
	 void runQuery(const QString& text);
	 bool fetchMore();
	 void fetchMetadata(int index);
	 void abortQuery();
	 void setCurrentSource(int index);
	 /** Download URL. Used for handling redirection in external URLs */
	 QNetworkReply* download(const QUrl& url);
private slots:
	 void replyFinished(QNetworkReply *reply);
	 void rerunQuery();
signals:
	 void connected(const QStringList& sources);
	 void serverError(const QString& message);
	 void corpusChanged();
	 void queryDone(const QString& matches);
	 void metadataReceived();
private:
	 enum Operation {InvalidOperation, ConnectOperation, QueryOperation, SourceOperation, MetadataOperation, SettingsOperation};

	 /** Create a basic network request. */
	 QNetworkRequest request(const QString &type, const QUrl &url);

	 void clearQuery();
	 void connectionFinished(QNetworkReply *reply);
	 void selectSourceFinished(QNetworkReply *reply);

	 bool parseReply(Operation operation, QNetworkReply* reply);
	 bool parseSources(QNetworkReply* device);
	 bool parseQuery(QNetworkReply* reply);
	 bool parseMetadata(QNetworkReply* device);
	 /** @return text between two tags. */
	 QString textBetweenTags(const QString& body, const QString& startTag, const QString &endTag);

	 QNetworkAccessManager* m_network;
	 QMap<Operation, QNetworkReply*> m_replies;

	 QUrl m_serverUrl;
	 QUrl m_nextQueries;
	 QUrl m_pendingQuery;

	 QStringList m_sources;
	 QString m_serverDescription;
	 QString m_corpusDescription;

	 QList<DjVuLink> m_results;
	 int m_matchesFound;
	 int m_currentSource;
	 bool m_configured;

	 Log m_logs;
};

#endif // POLIQARP_H
