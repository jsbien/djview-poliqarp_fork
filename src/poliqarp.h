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
	void query(const QString& text);
	void nextQuery();
	void previousQuery();
	void setCurrentSource(int index);
	int queryCount() const	{return qMin(m_queries.count() - m_currentQuery,int(QuerySize));}
	int matchesFound() const {return m_queryCount;}
	int firstMatchIndex() const	{return m_currentQuery;}
	bool hasNextQueries() const	{return m_currentQuery + QuerySize < m_queryCount;}
	bool hasPreviousQueries() const	{return m_currentQuery > 0;}
	DjVuLink query(int index)	const;
	QUrl serverUrl() const	{return m_serverUrl;}
private slots:
	void replyFinished(QNetworkReply *reply);
	void connectionFinished(QNetworkReply *reply);
	void queryFinished(QNetworkReply *reply);
	void selectSourceFinished(QNetworkReply *reply);

signals:
	void connected(const QStringList& sources);
	void connectionError(const QString& message);
	void sourceSelected();
	void queryFinished();

private:
	bool parseSources(QIODevice* device);
	bool parseQuery(QIODevice* device);
	QNetworkAccessManager* m_network;
	QNetworkReply* m_lastConnection;
	QNetworkReply* m_lastQuery;
	QNetworkReply* m_lastSource;
	QUrl m_serverUrl;
	QUrl m_nextQueries;
	QStringList m_sources;
	QList<DjVuLink> m_queries;
	int m_queryCount;
	int m_currentQuery;
	int m_currentSource;
	QString m_lastQueryText;
};

#endif // POLIQARP_H
