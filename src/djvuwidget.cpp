/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtGui>
#include "djvuwidget.h"

DjVuWidget::DjVuWidget(QWidget *parent) :
	QDjVuWidget(parent)
{
	m_document = 0;
}

DjVuWidget::~DjVuWidget()
{
	setDocument(0);
	delete m_document;
}

void DjVuWidget::setLink(const DjVuLink &link)
{
	m_link = link;
	if (m_document) {
		setDocument(0);
		delete m_document;
	}
	m_document = new QDjVuHttpDocument(this);
	if (m_document->setUrl(context(), m_link.link()))
		setDocument(m_document);
	connect(m_document, SIGNAL(docinfo()), this, SLOT(documentLoaded()));
}

void DjVuWidget::documentLoaded()
{
	if (!m_link.isValid())
		return;

	QDjVuWidget::Position pos;
	pos.pageNo = m_link.page();
	pos.inPage = true;
	pos.posPage = m_link.highlighted().topLeft();

	setPosition(pos, QPoint(width() / 2, height() / 2));

	QSettings settings;
	QColor color(settings.value("Display/highlight", "#ffff00").toString());
	color.setAlpha(96);
	addHighlight(m_link.page(), m_link.highlighted().left(),
					 m_link.highlighted().top(),
					 m_link.highlighted().width(),
					 m_link.highlighted().height(), color);
}

QDjVuContext *DjVuWidget::context()
{
	if (!m_context)
		m_context = new QDjVuContext("djview-poliqarp", QApplication::instance());
	return m_context;
}

QDjVuContext* DjVuWidget::m_context = 0;
