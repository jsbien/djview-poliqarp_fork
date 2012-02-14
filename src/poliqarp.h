/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARP_H
#define POLIQARP_H

#include <QtNetwork>

class Poliqarp : public QObject
{
	 Q_OBJECT
public:
	explicit Poliqarp(QObject *parent = 0);
public slots:
	void connectToServer(const QUrl& url);
	void query(const QString& text);
	void setCurrentSource(int index);
private slots:
	void replyFinished(QNetworkReply *reply);
	void connectionFinished(QNetworkReply *reply);
	void queryFinished(QNetworkReply *reply);
	void selectSourceFinished(QNetworkReply *reply);

signals:
	void connected(const QStringList& sources);
	void connectionError(const QString& message);
	void sourceSelected();

private:
	bool parseSources(QIODevice* device);
	QNetworkAccessManager* m_network;
	QNetworkReply* m_lastConnection;
	QNetworkReply* m_lastQuery;
	QNetworkReply* m_lastSource;
	QUrl m_url;
	QStringList m_sources;
	int m_currentSource;
};

#endif // POLIQARP_H
