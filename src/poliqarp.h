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
	void connectToServer(const QUrl& url);
	QStringList sources() const	{return m_sources;}
signals:
	void connected();
	void connectionError(const QString& message);
private slots:
	void replyFinished(QNetworkReply *reply);

private:
	bool parseSources(QIODevice* device);
	QNetworkAccessManager* m_network;
	QNetworkReply* m_lastConnection;
	QStringList m_sources;

};

#endif // POLIQARP_H
