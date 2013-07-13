/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

