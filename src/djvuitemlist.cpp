/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "djvuitemlist.h"
#include "qdjvuwidget.h"
#include "qdjvuhttp.h"

DjVuItemList::DjVuItemList(QWidget *parent) :
	 QWidget(parent)
{
	 setLayout(m_layout = new QGridLayout);
	 m_context = new QDjVuContext("Demo", this);
	 m_currentItem = -1;
}

void DjVuItemList::clear()
{
	 for (int i = 0; i < m_items.count(); i++) {
		  layout()->removeWidget(m_items[i].label);
		  layout()->removeWidget(m_items[i].label);
		  m_items[i].label->deleteLater();
		  m_items[i].djvu->setDocument(0);
		  m_items[i].djvu->deleteLater();
		  m_items[i].document->deleteLater();
	 }
	 m_items.clear();
	 m_currentItem = -1;
}

void DjVuItemList::addItem(const DjVuLink& link)
{
	 int row = m_items.count();

	 DjVuItem item;
	 item.label = new QLabel(QString::number(row + 1));

	 item.djvu = new QDjVuWidget(this);
	 item.djvu->setMaximumHeight(40);
	 item.djvu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	 item.djvu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	 item.djvu->viewport()->installEventFilter(this);

	 m_layout->addWidget(item.label, row, 0);
	 m_layout->addWidget(item.djvu, row, 1);

	 item.document = new QDjVuHttpDocument(this);
	 if (item.document->setUrl(m_context, link.link()))
		  item.djvu->setDocument(item.document);
	 connect(item.document, SIGNAL(docinfo()), this, SLOT(documentLoaded()));

	 item.link = link;

	 m_items.append(item);

}

void DjVuItemList::documentLoaded()
{
	 QDjVuDocument* document = dynamic_cast<QDjVuDocument*>(sender());
	 for (int i = 0; i < m_items.count(); i++)
		  if (m_items[i].document == document) {
				showDocument(i);
				break;
		  }
}

void DjVuItemList::showDocument(int index)
{
	 DjVuLink link = m_items[index].link;
	 QDjVuWidget::Position pos;
	 pos.pageNo = link.page();
	 pos.inPage = true;
	 pos.posPage = link.highlighted().topLeft();

	 m_items[index].djvu->setPosition(pos, QPoint(m_items[index].djvu->width() / 2,
																 m_items[index].djvu->height() / 2));

	 QSettings settings;
	 QColor color(settings.value("Display/highlight", "#ffff00").toString());
	 color.setAlpha(96);
	 m_items[index].djvu->addHighlight(link.page(),
												  link.highlighted().left(),
												  link.highlighted().top(),
												  link.highlighted().width(),
												  link.highlighted().height(), color);
}


bool DjVuItemList::eventFilter(QObject *widget, QEvent *event)
{
	if (event->type() != QEvent::MouseButtonPress)
		return false;

	for (int i = 0; i < m_items.count(); i++)
		if (m_items[i].djvu->viewport() == widget && i != m_currentItem)
			emit documentRequested(m_items[m_currentItem = i].link);
	 return false;
}

