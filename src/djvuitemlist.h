/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVUITEMLIST_H
#define DJVUITEMLIST_H

#include "djvupreview.h"
#include "djvulink.h"
#include "qdjvuhttp.h"
#include <QtGui>

class DjVuItemList : public QScrollArea
{
	 Q_OBJECT
public:
	 explicit DjVuItemList(QWidget *parent = 0);
	 void addItem(const DjVuLink& link);
	 void clear();
	 int count() const   {return m_items.count();}
	 int currentIndex() const {return m_currentItem;}
protected:
	 bool eventFilter(QObject* object, QEvent* event);
public slots:
	 void setCurrentIndex(int index);
	 void configure();
private slots:
	 void updateCurrentItem();
	 void showMetadata(int index);
signals:
	 void currentIndexChanged(int index);
	 void metadataActivated(int index);
	 void documentRequested(const DjVuLink& link);
private:
	 struct DjVuItem{
		  QLabel* label;
		  DjVuPreview* djvu;
	 };

	 int m_currentItem;
	 QGridLayout* m_layout;
	 QVector<DjVuItem> m_items;
	 QDjVuContext* m_context;
	 QWidget* m_mainWidget;
};

#endif // DJVUITEMLIST_H
