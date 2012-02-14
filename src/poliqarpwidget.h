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
	void doSearch();
	void connected();
	void connectionError(const QString& message);
private:
	Ui::PoliqarpWidget ui;
	Poliqarp* m_poliqarp;
};

#endif // POLIQARPWIDGET_H
