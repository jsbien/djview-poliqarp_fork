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

#ifndef ALIGNEDITEMDELEGATE_H
#define ALIGNEDITEMDELEGATE_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#  include <QtWidgets>
#else
#  include <QtGui>
#endif

class AlignedItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit AlignedItemDelegate(QObject *parent = 0);
protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // ALIGNEDITEMDELEGATE_H
