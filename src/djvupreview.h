/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
protected:
	void focusInEvent(QFocusEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
signals:
	void activated();
	void documentRequested(const DjVuLink& link);
private:
	QVariant m_data;
};

#endif // DJVUPREVIEW_H
