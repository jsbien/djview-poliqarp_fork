/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARPWIDGET_H
#define POLIQARPWIDGET_H

#include "ui_poliqarpwidget.h"

class Poliqarp;

class PoliqarpWidget : public QWidget
{
	 Q_OBJECT
public:
	explicit PoliqarpWidget(QWidget *parent = 0);
private slots:
	void doConnect();
	void doSelectSource();
	void doSearch();
	void connected(const QStringList& sources);
	void connectionError(const QString& message);
	void sourceSelected();
	void updateQueries();
	void showDocument();
signals:
	void documentRequested(const QUrl& url);
private:
	Ui::PoliqarpWidget ui;
	Poliqarp* m_poliqarp;
};

#endif // POLIQARPWIDGET_H
