/****************************************************************************
*   Copyright (C) 2013-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#include "aligneditemdelegate.h"

AlignedItemDelegate::AlignedItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

void AlignedItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem option2 = option;
	Qt::Alignment alignment(index.data(Qt::TextAlignmentRole).toInt());
	if (alignment.testFlag(Qt::AlignRight))
		option2.textElideMode = Qt::ElideLeft;
	else if (alignment.testFlag(Qt::AlignLeft))
		option2.textElideMode = Qt::ElideRight;
	else if (alignment.testFlag(Qt::AlignHCenter))
		option2.textElideMode = Qt::ElideMiddle;
	QStyledItemDelegate::paint(painter, option2, index);
}

