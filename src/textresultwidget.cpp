/****************************************************************************
*   This software is subject to, and may be distributed under, the
*   GNU General Public License, either version 2 of the license,
*   or (at your option) any later version. The license should have
*   accompanied the software or you may obtain a copy of the license
*   from the Free Software Foundation at http://www.fsf.org .

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
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
