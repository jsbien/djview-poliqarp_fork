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

void DjVuPreview::focusInEvent(QFocusEvent *)
{
	emit gotFocus();
}

void DjVuPreview::mouseDoubleClickEvent(QMouseEvent*)
{
	emit doubleClicked();
}
