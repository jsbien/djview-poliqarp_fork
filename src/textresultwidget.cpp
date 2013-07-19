/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "textresultwidget.h"

TextResultWidget::TextResultWidget(QWidget *parent) :
	QTableWidget(parent)
{
}

void TextResultWidget::resizeEvent(QResizeEvent *event)
{
	QTableWidget::resizeEvent(event);
	updateColumnWidths();
}

void TextResultWidget::updateColumnWidths()
{
	QHeaderView* header = horizontalHeader();
	resizeColumnToContents(1);
	int sizeLeft = header->width() - header->sectionSize(1) - 20;
	header->resizeSection(0, sizeLeft / 2 - 2);
}
