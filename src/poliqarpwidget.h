/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARPWIDGET_H
#define POLIQARPWIDGET_H

#include "ui_poliqarpwidget.h"

class DjVuLink;
class Poliqarp;

class PoliqarpWidget : public QWidget
{
	  Q_OBJECT
public:
	 explicit PoliqarpWidget(QWidget *parent = 0);
	 ~PoliqarpWidget();
public slots:
	 void connectToServer();
	 void clear();
private slots:
	 void doSelectSource();
	 void doSearch();
	 void connected(const QStringList& sources);
	 void connectionError(const QString& message);
	 void sourceSelected(const QString& info);
	 void updateQueries();
	 void showDocument(const QModelIndex& index);
	 /** User switched between text and graphical mode. */
	 void displayModeChanged();
	 void synchronizeSelection();
signals:
	 void documentRequested(const DjVuLink& link);
	 void sourceUpdated(const QString& info);
private:
	 void updateTextQueries();
	 void updateGraphicalQueries();
	 void adjustTextColumns();
	 Ui::PoliqarpWidget ui;
	 Poliqarp* m_poliqarp;
};

#endif // POLIQARPWIDGET_H
