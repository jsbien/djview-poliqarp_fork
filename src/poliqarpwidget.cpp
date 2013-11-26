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

#include "poliqarpwidget.h"
#include "poliqarp.h"
#include "poliqarpsettingsdialog.h"
#include "messagedialog.h"
#include "djvuitemlist.h"

PoliqarpWidget::PoliqarpWidget(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);

	if (ui.queryCombo->completer())
		ui.queryCombo->completer()->setCaseSensitivity(Qt::CaseSensitive);

	// Connections and corpus selection
	connect(ui.serverCombo, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(connectToServer()));
	connect(ui.configureServerButton, SIGNAL(clicked()), this, SLOT(configureCorpus()));
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
	connect(ui.corpusCombo, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(doSelectSource()));

	connect(ui.serverInfoButton, SIGNAL(clicked()), this, SLOT(showServerDescription()));
	connect(ui.corpusInfoButton, SIGNAL(clicked()), this, SLOT(showCorpusDescription()));

	// Text items
	connect(ui.textResultTable, SIGNAL(doubleClicked(QModelIndex)), this,
			  SLOT(showDocument(QModelIndex)));
	connect(ui.textResultTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
			  this, SLOT(synchronizeSelection()));
	connect(ui.textResultTable->verticalHeader(), SIGNAL(sectionClicked(int)), this,
			  SLOT(metadataRequested()));

	// Graphical items
	connect(ui.graphicalResultList, SIGNAL(documentRequested(DjVuLink)), this,
			  SIGNAL(documentRequested(DjVuLink)));
	connect(ui.graphicalResultList, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(synchronizeSelection()));
	connect(ui.graphicalResultList, SIGNAL(metadataActivated(int)), this,
			  SLOT(metadataRequested()));

	// Searches
	connect(ui.queryCombo->lineEdit(), SIGNAL(returnPressed()), this,
			  SLOT(doSearch()));
	connect(ui.resultWidget, SIGNAL(currentChanged(int)), this,
			  SLOT(fetchMetadata()));

	// Metadata
	connect(ui.metadataBrowser, SIGNAL(anchorClicked(QUrl)), this,
			  SLOT(metadataLinkOpened(QUrl)));
	connect(ui.nextMetadataButton, SIGNAL(clicked()), this,
			  SLOT(nextMetadata()));
	connect(ui.previousMetadataButton, SIGNAL(clicked()), this,
			  SLOT(previousMetadata()));
	connect(ui.removeCurrentButton, SIGNAL(clicked()), this,
			  SLOT(hideCurrentItem()));


	// Poliqarp connections
	m_poliqarp = new Poliqarp(this);
	connect(m_poliqarp, SIGNAL(connected(QStringList)), this,
			  SLOT(connected(QStringList)));
	connect(m_poliqarp, SIGNAL(serverError(QString)), this,
			  SLOT(showError(QString)));
	connect(m_poliqarp, SIGNAL(corpusChanged()), this,
			  SLOT(corpusChanged()));
	connect(m_poliqarp, SIGNAL(queryDone(QString)), this,
			  SLOT(updateQueries(QString)));
	connect(m_poliqarp, SIGNAL(metadataReceived()), this,
			  SLOT(metadataReceived()));
	connect(ui.moreButton, SIGNAL(clicked()), m_poliqarp, SLOT(fetchMore()));

	// Removing items
	connect(ui.actionResultResult, SIGNAL(triggered()), this, SLOT(hideCurrentItem()));
	connect(ui.graphicalResultList, SIGNAL(hideCurrent()), this, SLOT(hideCurrentItem()));
	ui.textResultTable->addAction(ui.actionResultResult);
	addAction(ui.actionResultResult);

	// Index
	connect(ui.indexWidget, SIGNAL(indexOpened()), this, SIGNAL(indexOpened()));
	connect(ui.indexWidget, SIGNAL(indexClosed()), this, SIGNAL(indexClosed()));
	connect(ui.indexWidget, SIGNAL(documentRequested(DjVuLink)), this,
			  SIGNAL(documentRequested(DjVuLink)));

	// Logging
	connect(this, SIGNAL(documentRequested(DjVuLink)), this, SLOT(logDocument(DjVuLink)));

	QSettings settings;
	settings.beginGroup("Poliqarp");
	ui.queryCombo->addItems(settings.value("queries").toStringList());
	ui.queryCombo->clearEditText();
	settings.endGroup();

	configure();
	clear();
}


PoliqarpWidget::~PoliqarpWidget()
{
	QSettings settings;
	settings.beginGroup("Poliqarp");
	QStringList items;
	for (int i = 0; i < ui.queryCombo->count(); i++)
		items.append(ui.queryCombo->itemText(i));
	settings.setValue("queries", items);
	settings.setValue("server", ui.serverCombo->currentIndex());
	settings.endGroup();
}

QStringList PoliqarpWidget::logs() const
{
	return m_poliqarp->logs();
}

void PoliqarpWidget::clearLog()
{
	m_poliqarp->clearLog();
}

void PoliqarpWidget::connectToServer()
{
	QUrl url;
	url.setHost(ui.serverCombo->currentText());

	url.setScheme("http");
	if (!url.isValid()) {
		MessageDialog::warning(tr("This URL is not valid"));
		return;
	}
	ui.corpusCombo->clear();
	setSearching(false);
	clear();
	setCursor(QCursor(Qt::WaitCursor));
	m_poliqarp->connectToServer(url);
}

void PoliqarpWidget::doSearch()
{
	if (isSearching())
		m_poliqarp->abortQuery();
	else m_poliqarp->runQuery(ui.queryCombo->currentText());
	clear();
	setSearching(!isSearching());
}

void PoliqarpWidget::doSelectSource()
{
	setSearching(false);
	ui.searchButton->setEnabled(false);
	clear();
	m_poliqarp->setCurrentSource(ui.corpusCombo->currentIndex());
}

void PoliqarpWidget::connected(const QStringList& sources)
{
	unsetCursor();
	ui.corpusCombo->clear();
	ui.corpusCombo->addItems(sources);
	ui.corpusCombo->setEnabled(true);
	QSettings settings;
	int lastIndex = settings.value(QString("Poliqarp/") +
											 m_poliqarp->serverUrl().host(), 0).toInt();
	ui.corpusCombo->setCurrentIndex(lastIndex);
}

void PoliqarpWidget::showError(const QString &message)
{
	unsetCursor();
	MessageDialog::warning(message, tr("Server error"));
}

void PoliqarpWidget::hideCurrentItem()
{
	int row = ui.textResultTable->currentRow();
	if (row == -1)
		return;
	ui.textResultTable->verticalHeader()->setSectionHidden(row, true);
	ui.graphicalResultList->setItemVisible(row, false);
	int newRow = nextVisibleItem(row - 1);
	if (newRow == -1)
		newRow = previousVisibleItem(row);
	if (newRow != -1)
		ui.textResultTable->selectRow(newRow);
	fetchMetadata();
}

void PoliqarpWidget::addEntry(const QString &word, const QUrl &link)
{
	ui.indexWidget->addEntry(word, link);
}

void PoliqarpWidget::updateCurrentEntry(const QUrl &link)
{
	ui.indexWidget->updateCurrentEntry(link);
}

void PoliqarpWidget::corpusChanged()
{
	ui.searchButton->setEnabled(true);
	ui.queryCombo->setFocus();
	QSettings settings;
	settings.setValue(QString("Poliqarp/") + m_poliqarp->serverUrl().host(),
							ui.corpusCombo->currentIndex());
	QString corpus = m_poliqarp->currentSource().section('/', -2, -2);
	emit corpusSelected(corpus);
	emit informationReceived(m_poliqarp->corpusDescription());

	ui.indexWidget->open(m_poliqarp->corpusUrl().toString());
}

void PoliqarpWidget::metadataReceived()
{
	QString metadata = m_poliqarp->result(ui.textResultTable->currentIndex().row()).metadata();
	if (!metadata.isEmpty())
		ui.metadataBrowser->setHtml(metadata);
}

void PoliqarpWidget::metadataRequested()
{
	ui.resultWidget->setCurrentWidget(ui.metadataTab);
}

void PoliqarpWidget::metadataLinkOpened(const QUrl& url)
{
	QNetworkReply* reply = m_poliqarp->download(url);
	connect(reply, SIGNAL(finished()), this, SLOT(openUrl()));
}

void PoliqarpWidget::nextMetadata()
{
	int row = nextVisibleItem(ui.textResultTable->currentRow());
	if (row != -1) {
		ui.textResultTable->selectRow(row);
		fetchMetadata();
	}
}

void PoliqarpWidget::previousMetadata()
{
	int row = previousVisibleItem(ui.textResultTable->currentRow());
	if (row != -1) {
		ui.textResultTable->selectRow(row);
		fetchMetadata();
	}
}

void PoliqarpWidget::updateQueries(const QString& message)
{
	setSearching(false);
	updateTextQueries();
	updateGraphicalQueries();
	ui.matchLabel->setText(message);

	ui.moreButton->setEnabled(m_poliqarp->hasMore());

	if (ui.textResultTable->currentRow() == -1 && ui.textResultTable->rowCount() > 0) {
		ui.textResultTable->setCurrentIndex(ui.textResultTable->model()->index(0, 0));
		if (ui.resultWidget->currentWidget() == ui.metadataTab)
			m_poliqarp->fetchMetadata(ui.textResultTable->currentIndex().row());
	}
}

void PoliqarpWidget::showDocument(const QModelIndex& index)
{
	ui.textResultTable->updateColumnWidths();
	if (index.isValid()) {
		DjVuLink item = m_poliqarp->result(index.row());
		if (item.link().isValid())
			emit documentRequested(item);
	}
}

void PoliqarpWidget::showServerDescription()
{
	emit informationReceived(m_poliqarp->serverDescription());
}

void PoliqarpWidget::showCorpusDescription()
{
	emit informationReceived(m_poliqarp->corpusDescription());
}


void PoliqarpWidget::fetchMetadata()
{
	ui.nextMetadataButton->setEnabled(nextVisibleItem(ui.textResultTable->currentRow()) != -1);
	ui.previousMetadataButton->setEnabled(previousVisibleItem(ui.textResultTable->currentRow()) != -1);
	if (ui.resultWidget->currentWidget() == ui.metadataTab) {
		ui.metadataBrowser->clear();
		int row = ui.textResultTable->currentRow();
		if (isItemVisible(row))
			m_poliqarp->fetchMetadata(row);
	}
}

void PoliqarpWidget::synchronizeSelection()
{
	int textRow = ui.textResultTable->currentRow();
	int graphicRow = ui.graphicalResultList->currentIndex();

	if (textRow == graphicRow)
		return;
	else if (ui.resultWidget->currentWidget() != ui.graphicalTab)
		ui.graphicalResultList->setCurrentIndex(textRow);
	else {
		QModelIndex current = ui.textResultTable->model()->index(graphicRow, 0);
		ui.textResultTable->setCurrentIndex(current);
	}
}

void PoliqarpWidget::openUrl()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if (!reply)
		return;

	QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	QUrl url = redirection.isValid() ? redirection : reply->url();
	if (url.path().contains(".djvu")) {
		QString urlString = url.toString();
		// Append highlight color to DjVu link. It cannot be done earlier because redirection fails - see #86
		if (urlString.section('&', -1).startsWith("highlight=")) {
			QColor highlight(QSettings().value("Display/highlight", "#ffff00").toString());
			urlString.append(QString(",%1").arg(highlight.name().mid(1)));
			url = QUrl(urlString);
		}
		QString cmd = QSettings().value("Tools/djviewPath", "djview").toString();
		QStringList args;
		args << url.toString();
		if (!QProcess::startDetached(cmd, args)) {
			QString msg = tr("Cannot execute program:") + "<br><i>%1</i>";
			MessageDialog::warning(msg.arg(cmd));
		}
	}
	else QDesktopServices::openUrl(url);
	reply->deleteLater();
}

void PoliqarpWidget::logDocument(const DjVuLink& link)
{
	if (link.isValid())
		m_poliqarp->log("djvu", QString("%1, page %2")
							 .arg(link.link().toString())
							 .arg(link.page()));
}

void PoliqarpWidget::updateTextQueries()
{
	int oldCount = ui.textResultTable->rowCount();
	ui.textResultTable->setRowCount(m_poliqarp->queryCount());

	// TODO: move it to text list class
	bool extraColumn = false;

	QFont font;
	font.fromString(QSettings().value("Display/textFont", font.toString()).toString());
	if (font.family().isEmpty())
		font = ui.textResultTable->font();
	ui.textResultTable->verticalHeader()->setDefaultSectionSize(QFontMetrics(font).height() + 2);

	QFont boldFont = font;
	boldFont.setBold(true);

	for (int i = oldCount; i < m_poliqarp->queryCount(); i++) {
		DjVuLink item = m_poliqarp->result(i);
		if (item.columns() == 4)
			extraColumn = true;

		QTableWidgetItem* left = new QTableWidgetItem(item.leftContext());
		left->setFont(font);
		left->setTextAlignment(Qt::AlignRight);
		ui.textResultTable->setItem(i, 0, left);

		QTableWidgetItem* match = new QTableWidgetItem(item.match());
		match->setTextAlignment(Qt::AlignLeft);
		match->setFont(boldFont);
		match->setForeground(Qt::darkBlue);
		ui.textResultTable->setItem(i, 1, match);

		QTableWidgetItem* extraMatch = new QTableWidgetItem(item.rightMatch());
		extraMatch->setTextAlignment(Qt::AlignLeft);
		extraMatch->setFont(boldFont);
		extraMatch->setForeground(Qt::darkBlue);
		ui.textResultTable->setItem(i, 2, extraMatch);

		QTableWidgetItem* right = new QTableWidgetItem(item.rightContext());
		right->setFont(font);
		right->setTextAlignment(Qt::AlignLeft);
		ui.textResultTable->setItem(i, 3, right);
	}
	if (m_poliqarp->queryCount() > 0) {
		QString text = ui.queryCombo->currentText();
		int old = ui.queryCombo->findText(text);
		if (old != -1)
			ui.queryCombo->removeItem(old);
		else if (ui.queryCombo->count() >= 20)
			ui.queryCombo->removeItem(ui.queryCombo->count() - 1);
		ui.queryCombo->insertItem(0, text);
		ui.queryCombo->setCurrentIndex(0);
	}

	ui.textResultTable->horizontalHeader()->setSectionHidden(2, !extraColumn);
	ui.textResultTable->updateColumnWidths();
}

void PoliqarpWidget::updateGraphicalQueries()
{
	// Graphical results
	int oldCount = ui.graphicalResultList->count();
	for (int i = oldCount; i < m_poliqarp->queryCount(); i++)
		ui.graphicalResultList->addItem(m_poliqarp->result(i));
}

int PoliqarpWidget::nextVisibleItem(int current) const
{
	for (int row = current + 1; row < ui.textResultTable->rowCount(); row++)
		if (isItemVisible(row))
			return row;
	return -1;
}

int PoliqarpWidget::previousVisibleItem(int current) const
{
	for (int row = current - 1; row >= 0; row--)
		if (isItemVisible(row))
			return row;
	return -1;
}

bool PoliqarpWidget::isItemVisible(int row) const
{
	return row >= 0 && row < ui.textResultTable->rowCount() &&
			!ui.textResultTable->verticalHeader()->isSectionHidden(row);
}

void PoliqarpWidget::clear()
{
	ui.textResultTable->setRowCount(0);
	ui.textResultTable->horizontalHeader()->setSectionHidden(2, true);
	ui.graphicalResultList->clear();
	ui.metadataBrowser->clear();
	ui.matchLabel->clear();
	emit documentRequested(DjVuLink());
}

void PoliqarpWidget::configure()
{
	ui.graphicalResultList->configure();

	QStringList defaultServers;
	defaultServers << "poliqarp.wbl.klf.uw.edu.pl" << "poliqarp.kanji.klf.uw.edu.pl";

	QSettings settings;
	QString currentServer = ui.serverCombo->currentText();
	if (currentServer.isEmpty() && ui.serverCombo->count()) {
		int currentIndex = settings.value("server", 0).toInt();
		currentServer = ui.serverCombo->itemText(qBound(0, currentIndex, ui.serverCombo->count() - 1));
	}

	QStringList newServerList = settings.value("Poliqarp/servers", defaultServers).toStringList();
	QStringList oldServerList;
	for (int i = 0; i < ui.serverCombo->count(); i++)
		oldServerList.append(ui.serverCombo->itemText(i));
	if (newServerList != oldServerList) {
		// Update server list - clears result
		ui.serverCombo->clear();
		ui.serverCombo->addItems(newServerList);
		if (!currentServer.isEmpty())
			ui.serverCombo->setCurrentIndex(ui.serverCombo->findText(currentServer));
		if (ui.serverCombo->currentIndex() == -1 && ui.serverCombo->count())
			ui.serverCombo->setCurrentIndex(0);
	}
	else { // Update font if items exist
		ui.textResultTable->setRowCount(0);
		updateTextQueries();
	}
}

bool PoliqarpWidget::exportResults(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::Append))
		return false;
	QTextStream out(&file);
	out.setCodec("UTF-8");
	if (file.pos() == 0)
		out << "Number,Left context,Match,Match2,Right context,Link\n";
	for (int i = 0; i < m_poliqarp->queryCount(); i++)
		if (ui.graphicalResultList->isItemVisible(i))
			out << (i+1) << ',' << m_poliqarp->result(i).toCsv();
	return true;
}

void PoliqarpWidget::configureCorpus()
{
	PoliqarpSettingsDialog dlg(this);
	dlg.restoreSettings(m_poliqarp->corpusUrl());
	if (dlg.exec()) {
		dlg.saveSettings();
		m_poliqarp->updateSettings();
		ui.indexWidget->open(m_poliqarp->corpusUrl().toString());
	}
}

void PoliqarpWidget::setSearching(bool enabled)
{
	if (enabled) {
		ui.searchButton->setText(tr("Abort"));
		setCursor(QCursor(Qt::WaitCursor));
	}
	else {
		ui.searchButton->setText(tr("Search"));
		unsetCursor();
	}
}

bool PoliqarpWidget::isSearching() const
{
	return ui.searchButton->text() == tr("Abort");
}



