/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARPWIDGET_H
#define POLIQARPWIDGET_H

#include "ui_poliqarpwidget.h"
#include <QtNetwork>

class PoliqarpWidget : public QWidget
{
	 Q_OBJECT

public:
	explicit PoliqarpWidget(QWidget *parent = 0);
private slots:
	void doConnect();
	void doSearch();
	void replyFinished(QNetworkReply* reply);
private:
	bool parseSources(QIODevice* device);

	Ui::PoliqarpWidget ui;
	QNetworkAccessManager* m_network;
};

#endif // POLIQARPWIDGET_H
