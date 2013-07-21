/****************************************************************************
*   Copyright (C) 2012-2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#ifndef DJVUPREVIEW_H
#define DJVUPREVIEW_H

#include "djvulink.h"
#include "djvuwidget.h"

/** This is a small class to handle QDjVu previews. */

class DjVuPreview : public DjVuWidget
{
	Q_OBJECT
public:
	explicit DjVuPreview(QWidget *parent = 0);
	void setData(const QVariant& data) {m_data = data;}
	QVariant data() const {return m_data;}
public slots:
	void configure();
protected:
	void focusInEvent(QFocusEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *event);
signals:
	void activated();
	void documentRequested(const DjVuLink& link);
private:
	QVariant m_data;
};

#endif // DJVUPREVIEW_H
