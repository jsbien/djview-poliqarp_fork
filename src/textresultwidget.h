/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
