/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtGui>
#include "djvupreview.h"

DjVuPreview::DjVuPreview(QWidget *parent) :
	DjVuWidget(parent)
{
	setMaximumHeight(40);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	viewport()->installEventFilter(this);
	setFocusPolicy(Qt::StrongFocus);
}

void DjVuPreview::focusInEvent(QFocusEvent* event)
{
	if (event->reason() == Qt::TabFocusReason || event->reason() == Qt::MouseFocusReason)
		emit activated();
}

void DjVuPreview::mouseDoubleClickEvent(QMouseEvent*)
{
	if (link().isValid())
		emit documentRequested(link());
}

