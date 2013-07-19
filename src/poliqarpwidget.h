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
	/** @return log items. */
	QStringList logs() const;
public slots:
	 void connectToServer();
	 void clear();
	 void configure();
	 bool exportResults(const QString& filename);
private slots:
	 void configureServer();
	 void doSelectSource();
	 void doSearch();
	 void connected(const QStringList& sources);
	 void showError(const QString& message);
	 void sourceSelected(const QString& info);
	 void metadataReceived();
	 void metadataRequested();
	 void metadataLinkOpened(const QUrl& url);
	 void updateQueries(const QString& message);
	 void showDocument(const QModelIndex& index);
	 /** User switched between text and graphical mode. */
	 void displayModeChanged();
	 void synchronizeSelection();
signals:
	 void documentRequested(const DjVuLink& link);
	 void sourceChanged(const QString& title, const QString& info);

private:
	 void setSearching(bool enabled);
	 bool isSearching() const;
	 void updateTextQueries();
	 void updateGraphicalQueries();
	 Ui::PoliqarpWidget ui;
	 Poliqarp* m_poliqarp;
};

#endif // POLIQARPWIDGET_H
