/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "djvuitemlist.h"
#include "qdjvuwidget.h"
#include "qdjvuhttp.h"

DjVuItemList::DjVuItemList(QWidget *parent) :
	 QScrollArea(parent)
{
	QWidget* main = new QWidget;
	main->setLayout(m_layout = new QGridLayout);
	setWidget(main);
	setWidgetResizable(true);

	m_context = new QDjVuContext("Demo", this);
	m_currentItem = -1;
}

void DjVuItemList::clear()
{
	 for (int i = 0; i < m_items.count(); i++)
		 m_items[i].djvu->setDocument(0);

	 while (QLayoutItem* item = m_layout->takeAt(0)) {
		 delete item->widget();
		 delete item;
	 }

	 m_items.clear();
	 m_currentItem = -1;
}

bool DjVuItemList::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() != QEvent::MouseButtonPress)
		return false;
	for (int i = 0; i < m_items.count(); i++)
		if (m_items[i].label == object) {
			showMetadata(i);
			break;
		}
	return false;
}

void DjVuItemList::setCurrentIndex(int index)
{
	if (m_currentItem == index)
		return;
	if (m_currentItem != -1)
		m_items[m_currentItem].label->setStyleSheet("");
	if (index < 0 || index >= m_items.count()) {
		m_currentItem = -1;
		return;
	}

	QString style = QString("color: %1; background-color: %2; font-weight: bold")
			.arg(QApplication::palette().color(QPalette::HighlightedText).name())
			.arg(QApplication::palette().color(QPalette::Highlight).name());

	m_items[index].label->setStyleSheet(style);
	ensureWidgetVisible(m_items[index].djvu);
	emit currentIndexChanged(m_currentItem = index);
}

void DjVuItemList::addItem(const DjVuLink& link)
{
	 int row = m_items.count();

	 DjVuItem item;
	 item.label = new QLabel(QString(" %1 ")
									 .arg(row+1), widget());
	 item.label->installEventFilter(this);

	 item.djvu = new DjVuPreview(widget());
	 item.djvu->setData(m_items.count());
	 connect(item.djvu, SIGNAL(activated()), this, SLOT(updateCurrentItem()));
	 connect(item.djvu, SIGNAL(documentRequested(DjVuLink)),
				this, SIGNAL(documentRequested(DjVuLink)));

	 m_layout->addWidget(item.label, row, 0);
	 m_layout->addWidget(item.djvu, row, 1);

	 item.djvu->openLink(link);

	 m_items.append(item);
}

void DjVuItemList::updateCurrentItem()
{
	DjVuPreview* preview = dynamic_cast<DjVuPreview*>(sender());
	if (preview)
		setCurrentIndex(preview->data().toInt());
}

void DjVuItemList::showMetadata(int index)
{
	if (index >= 0) {
		setCurrentIndex(index);
		emit metadataActivated(index);
	}
}

void DjVuItemList::configure()
{
	for (int i = 0; i < count(); i++)
		m_items[i].djvu->configure();
}



