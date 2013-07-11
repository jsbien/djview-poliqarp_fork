/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARP_H
#define POLIQARP_H

#include <QtNetwork>
#include "djvulink.h"

class Poliqarp : public QObject
{
	  Q_OBJECT
private:
	 enum {QuerySize = 25};
public:
	 explicit Poliqarp(QObject *parent = 0);
public slots:
	 void connectToServer(const QUrl& url);
	 void runQuery(const QString& text);
	 bool fetchMore();
	 void fetchMetadata(int index);
	 void abortQuery();
	 void setCurrentSource(int index);
	 int queryCount() const	{return m_queries.count();}
	 int matchesFound() const {return m_matchesFound;}
	 bool hasMore() const	{return m_queries.count() < m_matchesFound;}
	 DjVuLink query(int index)	const;
	 QUrl serverUrl() const	{return m_serverUrl;}
	 QString currentSource() const {return m_sources[m_currentSource];}
	 QStringList logs() const {return m_logs;}
	 void updateSettings();
private slots:
	 void replyFinished(QNetworkReply *reply);
	 void rerunQuery();
signals:
	 void connected(const QStringList& sources);
	 void serverError(const QString& message);
	 void sourceSelected(const QString& info);
	 void queryDone(const QString& matches);
	 void metadataReceived();
private:
	 enum Operation {InvalidOperation, ConnectOperation, QueryOperation, SourceOperation, MetadataOperation, SettingsOperation};

	 void clearQuery();
	 void connectionFinished(QNetworkReply *reply);
	 void selectSourceFinished(QNetworkReply *reply);

	 bool parseReply(Operation operation, QNetworkReply* reply);
	 bool parseSources(QNetworkReply* device);
	 bool parseQuery(QNetworkReply* reply);
	 bool parseMetadata(QNetworkReply* device);
	 /** Create a basic network request. */
	 QNetworkRequest request(const QString &type, const QUrl &url);
	 /** @return text between two tags. */
	 QString textBetweenTags(const QString& body, const QString& startTag, const QString &endTag);

	 QNetworkAccessManager* m_network;
	 QMap<Operation, QNetworkReply*> m_replies;

	 QUrl m_serverUrl;
	 QUrl m_nextQueries;
	 QUrl m_pendingQuery;

	 QStringList m_sources;

	 QList<DjVuLink> m_queries;
	 int m_matchesFound;
	 int m_currentSource;
	 bool m_configured;

	 QStringList m_logs;
};

#endif // POLIQARP_H
