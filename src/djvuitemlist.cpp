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
		  delete m_items[i].document;
	 }
	 m_items.clear();
	 m_currentItem = -1;
}

void DjVuItemList::setCurrentItem(int index)
{
	if (m_currentItem == index)
		return;
	if (m_currentItem != -1)
		m_items[m_currentItem].label->setStyleSheet("");

	QString style = QString("color: %1; background-color: %2; font-weight: bold")
			.arg(QApplication::palette().color(QPalette::HighlightedText).name())
			.arg(QApplication::palette().color(QPalette::Highlight).name());

	m_items[index].label->setStyleSheet(style);
	m_currentItem = index;
}

void DjVuItemList::addItem(const DjVuLink& link)
{
	 int row = m_items.count();

	 DjVuItem item;
	 item.label = new QLabel(QString(" %1 ").arg(row + 1));

	 item.djvu = new DjVuPreview(this);
	 item.djvu->setData(m_items.count());
	 connect(item.djvu, SIGNAL(gotFocus()), this, SLOT(updateCurrentItem()));
	 connect(item.djvu, SIGNAL(documentRequested(DjVuLink)),
				this, SIGNAL(documentRequested(DjVuLink)));

	 m_layout->addWidget(item.label, row, 0);
	 m_layout->addWidget(item.djvu, row, 1);

	 item.document = new QDjVuHttpDocument(this);
	 if (item.document->setUrl(m_context, link.link()))
		  item.djvu->setDocument(item.document);
	 item.djvu->setLink(item.document, link);

	 m_items.append(item);
}

void DjVuItemList::updateCurrentItem()
{
	DjVuPreview* preview = dynamic_cast<DjVuPreview*>(sender());
	if (preview)
		setCurrentItem(preview->data().toInt());
}


