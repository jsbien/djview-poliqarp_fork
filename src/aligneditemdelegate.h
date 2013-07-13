/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef ALIGNEDITEMDELEGATE_H
#define ALIGNEDITEMDELEGATE_H

#include <QtGui>

class AlignedItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit AlignedItemDelegate(QObject *parent = 0);
protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // ALIGNEDITEMDELEGATE_H
