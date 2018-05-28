/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#include "ui_poliqarpwidget.h"

class DjVuLink;
class Poliqarp;

class PoliqarpWidget : public QWidget
{
	  Q_OBJECT
public:
	 explicit PoliqarpWidget(QWidget* parent = nullptr);
	 ~PoliqarpWidget();
	/** @return log items. */
	QStringList logs() const;
	/** Clear log. */
	void clearLog();
public slots:
	 void connectToServer();
	 void clear();
	 void configure();
	 bool exportResults(const QString& filename);
	 void hideCurrentItem();
private slots:
	 // Server and corpus
	 void configureCorpus();
	 void doSelectSource();
	 void doSearch();
	 void connected(const QStringList& sources);
	 void showError(const QString& message);
	 void corpusChanged();

	 // Metadata
	 void metadataReceived();
	 void metadataRequested();
	 void metadataLinkOpened(const QUrl &url);
	 void nextMetadata();
	 void previousMetadata();

	 void updateQueries(const QString& message);

	 // Main panel
	 void showDocument(const QModelIndex& index);
	 /** Show server description as parsed by Poliqarp. */
	 void showServerDescription();
	 /** Show server description as parsed by Poliqarp. */
	 void showCorpusDescription();
	 /** Fetch metadata if the metadata tab is active. */
	 void fetchMetadata();

	 void synchronizeSelection();
	 /** Test external URL for redirection. */
	 void openUrl();

	 // Logging
	 void logDocument(const DjVuLink& link);
signals:
	 void documentRequested(const DjVuLink& link);
	 void corpusSelected(const QString& name);
	 /** Information in HTML format (server or corpus description) was requested. */
	 void informationReceived(const QString& html);
	 /** Status bar message. */
	 void statusMessage(const QString& message);


private:
	 void setSearching(bool enabled);
	 bool isSearching() const;
	 void updateTextQueries();
	 void updateGraphicalQueries();
	 /** @return index of next visible item in match list. */
	 int nextVisibleItem(int current) const;
	 /** @return index of previous visible item in match list. */
	 int previousVisibleItem(int current) const;
	 /** Check if given row is visible. */
	 bool isItemVisible(int row) const;
	 Ui::PoliqarpWidget ui;
	 Poliqarp* m_poliqarp;
};

