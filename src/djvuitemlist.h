/****************************************************************************
*   Copyright (C) 2012-2018 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#pragma once
#include "djvupreview.h"
#include "djvulink.h"
#include "qdjvunet.h"
#include <QtWidgets>

class DjVuItemList : public QScrollArea
{
	 Q_OBJECT
public:
	 explicit DjVuItemList(QWidget *parent = nullptr);
	 void addItem(const DjVuLink& link);
	 void showItem(int i) {setItemVisible(i, true);}
	 void hideCurrent(int i)  {setItemVisible(i, false);}
	 void setItemVisible(int i, bool visible);
	 bool isItemVisible(int i) const {return m_items[i].visible;}
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
	 void hideCurrent();
private:
	 struct DjVuItem{
		  QLabel* label;
		  DjVuPreview* djvu;
		  bool visible;
		  DjVuItem() {visible = true; label = nullptr; djvu = nullptr;}
	 };

	 int m_currentItem;
	 QGridLayout* m_layout;
	 QVector<DjVuItem> m_items;
	 QDjVuContext* m_context;
	 QWidget* m_mainWidget;

	 QAction* m_actionRemoveResult;
};

