/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "djvuitemdelegate.h"
#include "poliqarp.h"

DjVuItemDelegate::DjVuItemDelegate(Poliqarp* poliqarp, QObject *parent) :
	QStyledItemDelegate(parent), m_poliqarp(poliqarp)
{
}

QSize DjVuItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}

void DjVuItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (option.state & QStyle::State_Selected)
		  painter->fillRect(option.rect, option.palette.highlight());
	QFont font = option.font;
	font.setBold(true);
	painter->setFont(font);
	painter->drawText(option.rect, index.data().toString());
}
