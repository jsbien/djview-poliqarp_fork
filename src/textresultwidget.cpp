/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "textresultwidget.h"
#include "aligneditemdelegate.h"

TextResultWidget::TextResultWidget(QWidget *parent) :
	QTableWidget(parent)
{
	setItemDelegate(new AlignedItemDelegate(this));
	verticalHeader()->setToolTip(tr("Click to see metadata"));
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
	if (!header->isSectionHidden(2)) {
		resizeColumnToContents(2);
		sizeLeft -= header->sectionSize(2);
	}
	header->resizeSection(0, sizeLeft / 2 - 2);
}
