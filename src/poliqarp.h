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
	 void fetchMore();
	 void setCurrentSource(int index);
	 int queryCount() const	{return m_queries.count();}
	 int matchesFound() const {return m_matchesFound;}
	 bool hasMore() const	{return m_queries.count() < m_matchesFound;}
	 DjVuLink query(int index)	const;
	 QUrl serverUrl() const	{return m_serverUrl;}
private slots:
	 void replyFinished(QNetworkReply *reply);
	 void rerunQuery();
signals:
	 void connected(const QStringList& sources);
	 void connectionError(const QString& message);
	 void sourceSelected(const QString& info);
	 void queryFinished();
private:
	 void connectionFinished(QNetworkReply *reply);
	 bool queryFinished(QNetworkReply *reply);
	 void selectSourceFinished(QNetworkReply *reply);

	 bool parseSources(QIODevice* device);
	 bool parseQuery(QIODevice* device);
	 /** Create a basic network request. */
	 QNetworkRequest request(const QUrl &url) const;
	 /** @return text between two tags. */
	 QString textBetweenTags(const QString& body, const QString& startTag, const QString &endTag);

	 QNetworkAccessManager* m_network;
	 QNetworkReply* m_lastConnection;
	 QNetworkReply* m_lastQuery;
	 QNetworkReply* m_lastSource;
	 QUrl m_serverUrl;
	 QUrl m_nextQueries;
	 QUrl m_pendingQuery;

	 QStringList m_sources;

	 QList<DjVuLink> m_queries;
	 int m_matchesFound;
	 int m_currentSource;
	 QString m_lastQueryText;
};

#endif // POLIQARP_H
