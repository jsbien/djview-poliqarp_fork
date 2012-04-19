/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVUPREVIEW_H
#define DJVUPREVIEW_H

#include "djvulink.h"

/** This is a small class to handle QDjVu previews. */

#include "qdjvuwidget.h"

class DjVuPreview : public QDjVuWidget
{
	Q_OBJECT
public:
	explicit DjVuPreview(QWidget *parent = 0);
	void setLink(QDjVuDocument* document, const DjVuLink& link);
	void setData(const QVariant& data) {m_data = data;}
	QVariant data() const {return m_data;}
protected:
	void focusInEvent(QFocusEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
signals:
	void gotFocus();
	void documentRequested(const DjVuLink& link);
private slots:
	void documentLoaded();
private:
	QVariant m_data;
	DjVuLink m_link;
};

#endif // DJVUPREVIEW_H
