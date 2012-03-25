/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVUITEMDELEGATE_H
#define DJVUITEMDELEGATE_H

#include <QtGui>

class Poliqarp;


class DjVuItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit DjVuItemDelegate(Poliqarp* poliqarp, QObject *parent = 0);
protected:
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
	Poliqarp* m_poliqarp;
};

#endif // DJVUITEMDELEGATE_H
