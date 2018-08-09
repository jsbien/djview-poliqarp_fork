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

#pragma once

#include <QtGlobal>
#include <QtWidgets>

class AlignedItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit AlignedItemDelegate(QObject *parent = nullptr);
protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
