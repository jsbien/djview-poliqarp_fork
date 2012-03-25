/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVUITEMLIST_H
#define DJVUITEMLIST_H

#include "qdjvuwidget.h"
#include "djvulink.h"
#include <QtGui>

class DjVuItemList : public QWidget
{
	Q_OBJECT
public:
	explicit DjVuItemList(QWidget *parent = 0);
	void addItem(const DjVuLink& link);
	void clear();
private:
	QVector<QDjVuWidget*> m_items;
	QVector<DjVuLink> m_links;
	QVector<QDjVuDocument*> m_documents;
	QDjVuContext* m_context;
private slots:
	void documentLoaded();
};

#endif // DJVUITEMLIST_H
