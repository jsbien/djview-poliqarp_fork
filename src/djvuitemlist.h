/****************************************************************************
*   Copyright (C) 2012-2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
*   This software is subject to, and may be distributed under, the
*   GNU General Public License, either version 2 of the license,
*   or (at your option) any later version. The license should have
*   accompanied the software or you may obtain a copy of the license
*   from the Free Software Foundation at http://www.fsf.org .

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
****************************************************************************/

#ifndef DJVUITEMLIST_H
#define DJVUITEMLIST_H

#include "djvupreview.h"
#include "djvulink.h"
#include "qdjvunet.h"
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
