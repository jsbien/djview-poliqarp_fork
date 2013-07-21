/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#ifndef TEXTRESULTWIDGET_H
#define TEXTRESULTWIDGET_H

#include <QtGui>

class TextResultWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit TextResultWidget(QWidget *parent = 0);
protected:
	void resizeEvent(QResizeEvent *event);
public slots:
	void updateColumnWidths();
signals:
	
	
};

#endif // TEXTRESULTWIDGET_H
