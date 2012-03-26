/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "djvuitemlist.h"
#include "qdjvuwidget.h"
#include "qdjvuhttp.h"

DjVuItemList::DjVuItemList(QWidget *parent) :
	 QWidget(parent)
{
	setLayout(new QVBoxLayout);
	m_context = new QDjVuContext("Demo", this);
	m_currentItem = -1;
}

void DjVuItemList::clear()
{
	for (int i = 0; i < m_items.count(); i++) {
		layout()->removeWidget(m_items[i]);
		m_items[i]->deleteLater();
		m_documents[i]->deleteLater();
	}
	m_items.clear();
	m_links.clear();
	m_documents.clear();
	m_currentItem = -1;
}

void DjVuItemList::addItem(const DjVuLink& link)
{
	QDjVuWidget* item = new QDjVuWidget(this);
	item->setMaximumHeight(40);
	item->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->viewport()->installEventFilter(this);
	layout()->addWidget(item);
	m_items.append(item);
	m_links.append(link);

	QDjVuHttpDocument* document = new QDjVuHttpDocument(this);
	if (document->setUrl(m_context, link.link()))
		item->setDocument(document);
	m_documents.append(document);
	connect(document, SIGNAL(docinfo()), this, SLOT(documentLoaded()));
}

void DjVuItemList::documentLoaded()
{
	int index = indexOfDocument(sender());
	if (index != -1)
		showDocument(index);
}

void DjVuItemList::showDocument(int index)
{
	DjVuLink link = m_links[index];
	QDjVuWidget::Position pos;
	pos.pageNo = m_links[index].page();
	pos.inPage = true;
	pos.posPage = m_links[index].highlighted().topLeft();
	m_items[index]->setPosition(pos, QPoint(m_items[index]->width() / 2, m_items[index]->height() / 2));

	QSettings settings;
	QColor color(settings.value("Display/highlight", "#ffff00").toString());
	color.setAlpha(96);
	m_items[index]->addHighlight(link.page(),
										 link.highlighted().left(),
										 link.highlighted().top(),
										 link.highlighted().width(),
										  link.highlighted().height(), color);
}


bool DjVuItemList::eventFilter(QObject *widget, QEvent *event)
{
	int index = indexOfWidget(widget->parent());
	if (index != -1 && index != m_currentItem && event->type() ==
		 QEvent::MouseButtonPress)
		emit documentRequested(m_links[m_currentItem = index]);
	return false;
}

int DjVuItemList::indexOfDocument(QObject *object)
{
	QDjVuDocument* document = dynamic_cast<QDjVuDocument*>(object);
	if (document)
		return m_documents.indexOf(document);
	else return -1;
}

int DjVuItemList::indexOfWidget(QObject *object)
{
	QDjVuWidget* widget = dynamic_cast<QDjVuWidget*>(object);
	if (widget)
		return m_items.indexOf(widget);
	else return -1;
}
