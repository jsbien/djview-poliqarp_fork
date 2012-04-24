/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVUWIDGET_H
#define DJVUWIDGET_H

#include "djvulink.h"
#include "qdjvuwidget.h"
#include "qdjvuhttp.h"

class DjVuWidget : public QDjVuWidget
{
	Q_OBJECT
public:
	explicit DjVuWidget(QWidget *parent = 0);
	~DjVuWidget();
	DjVuLink link() const {return m_link;}
	void closeDocument();
public slots:
	void openLink(const DjVuLink& link);
private slots:
	void documentLoaded();
signals:
	void loading(const DjVuLink& link);
	void loaded(const DjVuLink& link);
private:
	QDjVuContext* context();
	QDjVuHttpDocument* m_document;
	DjVuLink m_link;
	static QDjVuContext* m_context;
};

#endif // DJVUWIDGET_H
