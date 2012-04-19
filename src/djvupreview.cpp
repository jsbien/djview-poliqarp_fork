/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtGui>
#include "djvupreview.h"

DjVuPreview::DjVuPreview(QWidget *parent) :
	QDjVuWidget(parent)
{
	setMaximumHeight(40);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	viewport()->installEventFilter(this);
	setFocusPolicy(Qt::StrongFocus);
}

void DjVuPreview::setLink(QDjVuDocument* document, const DjVuLink &link)
{
	m_link = link;
	connect(document, SIGNAL(docinfo()), this, SLOT(documentLoaded()));
}

void DjVuPreview::focusInEvent(QFocusEvent *)
{
	emit gotFocus();
}

void DjVuPreview::mouseDoubleClickEvent(QMouseEvent*)
{
	if (m_link.isValid())
	emit documentRequested(m_link);
}

void DjVuPreview::documentLoaded()
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
