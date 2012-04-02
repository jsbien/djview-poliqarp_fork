/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVUITEMLIST_H
#define DJVUITEMLIST_H

#include "qdjvuwidget.h"
#include "djvulink.h"
#include "qdjvuhttp.h"
#include <QtGui>

class DjVuItemList : public QWidget
{
	 Q_OBJECT
public:
	 explicit DjVuItemList(QWidget *parent = 0);
	 void addItem(const DjVuLink& link);
	 void clear();
	 int count() const   {return m_items.count();}
protected:
	 bool eventFilter(QObject* widget, QEvent* event);
private slots:
	 void documentLoaded();
	 void showDocument(int index);
signals:
	 void documentRequested(const DjVuLink& link);
private:
	 struct DjVuItem{
		  QLabel* label;
		  QDjVuWidget* djvu;
		  DjVuLink link;
		  QDjVuHttpDocument* document;
	 };

	 int m_currentItem;
	 QGridLayout* m_layout;
	 QVector<DjVuItem> m_items;
	 QDjVuContext* m_context;
};

#endif // DJVUITEMLIST_H
