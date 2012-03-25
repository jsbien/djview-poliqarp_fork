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
}

void DjVuItemList::addItem(const DjVuLink& link)
{
	QDjVuWidget* item = new QDjVuWidget(this);
	item->setMaximumHeight(60);
	item->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	item->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	layout()->addWidget(item);
	m_items.append(item);
	m_links.append(link);

	QDjVuHttpDocument* document = new QDjVuHttpDocument(this);
	if (document->setUrl(m_context, link.link()))
		item->setDocument(document);
	m_documents.append(document);
}
