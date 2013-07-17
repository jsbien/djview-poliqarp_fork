/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "poliqarpwidget.h"
#include "poliqarp.h"
#include "poliqarpsettingsdialog.h"
#include "messagedialog.h"
#include "djvuitemlist.h"
#include "aligneditemdelegate.h"

PoliqarpWidget::PoliqarpWidget(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);
	if (ui.queryCombo->completer())
		ui.queryCombo->completer()->setCaseSensitivity(Qt::CaseSensitive);
	ui.textResultTable->verticalHeader()->setDefaultSectionSize(
				ui.textResultTable->verticalHeader()->fontMetrics().height() + 6);

	ui.textResultTable->setItemDelegate(new AlignedItemDelegate(this));

	connect(ui.serverCombo, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(connectToServer()));
	connect(ui.configureServerButton, SIGNAL(clicked()), this, SLOT(configureServer()));
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
	connect(ui.corpusCombo, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(doSelectSource()));
	connect(ui.textResultTable, SIGNAL(doubleClicked(QModelIndex)), this,
			  SLOT(showDocument(QModelIndex)));
	connect(ui.textResultTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
			  this, SLOT(synchronizeSelection()));
	connect(ui.textResultTable->verticalHeader(), SIGNAL(sectionClicked(int)), this,
			  SLOT(metadataRequested()));

	connect(ui.graphicalResultList, SIGNAL(documentRequested(DjVuLink)), this,
			  SIGNAL(documentRequested(DjVuLink)));
	connect(ui.graphicalResultList, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(synchronizeSelection()));
	connect(ui.graphicalResultList, SIGNAL(metadataActivated(int)), this,
			  SLOT(metadataRequested()));

	connect(ui.queryCombo->lineEdit(), SIGNAL(returnPressed()), this,
			  SLOT(doSearch()));
	connect(ui.resultWidget, SIGNAL(currentChanged(int)), this,
			  SLOT(displayModeChanged()));

	connect(ui.metadataBrowser, SIGNAL(anchorClicked(QUrl)), this,
			  SLOT(metadataLinkOpened(QUrl)));


	m_poliqarp = new Poliqarp(this);
	connect(m_poliqarp, SIGNAL(connected(QStringList)), this,
			  SLOT(connected(QStringList)));
	connect(m_poliqarp, SIGNAL(serverError(QString)), this,
			  SLOT(showError(QString)));
	connect(m_poliqarp, SIGNAL(sourceSelected(QString)), this,
			  SLOT(sourceSelected(QString)));
	connect(m_poliqarp, SIGNAL(queryDone(QString)), this,
			  SLOT(updateQueries(QString)));
	connect(m_poliqarp, SIGNAL(metadataReceived()), this,
			  SLOT(metadataReceived()));

	connect(ui.moreButton, SIGNAL(clicked()), m_poliqarp, SLOT(fetchMore()));

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

void PoliqarpWidget::sourceSelected(const QString& info)
{
	ui.searchButton->setEnabled(true);
	ui.queryCombo->setFocus();
	QSettings settings;
	settings.setValue(QString("Poliqarp/") + m_poliqarp->serverUrl().host(),
							ui.corpusCombo->currentIndex());
	emit sourceChanged(m_poliqarp->currentSource().section('/', -2, -2), info);
}

void PoliqarpWidget::metadataReceived()
{
	QString metadata = m_poliqarp->query(ui.textResultTable->currentIndex().row()).metadata();
	if (!metadata.isEmpty())
		ui.metadataBrowser->setHtml(metadata);
}

void PoliqarpWidget::metadataRequested()
{
	ui.resultWidget->setCurrentWidget(ui.metadataTab);
}

void PoliqarpWidget::metadataLinkOpened(const QUrl &url)
{
	if (url.path().contains(".djvu")) {
		QString cmd = QSettings().value("Tools/djviewPath", "djview").toString();
		QStringList args;
		args << url.toString();
		if (!QProcess::startDetached(cmd, args)) {
			QString msg = tr("Cannot execute program:") + "<br><i>%1</i>";
			MessageDialog::warning(msg.arg(cmd));
		}
	}
	else QDesktopServices::openUrl(url);
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
	QHeaderView* header = ui.textResultTable->horizontalHeader();
	ui.textResultTable->resizeColumnToContents(1);
	int sizeLeft = header->width() - header->sectionSize(1) - 20;
	header->resizeSection(0, sizeLeft / 2 - 2);
	header->resizeSection(2, sizeLeft / 2 - 2);
	if (index.isValid()) {
		DjVuLink item = m_poliqarp->query(index.row());
		if (item.link().isValid())
			emit documentRequested(item);
	}
}

void PoliqarpWidget::displayModeChanged()
{
	if (ui.resultWidget->currentWidget() == ui.textTab)
		adjustTextColumns();
	else if (ui.resultWidget->currentWidget() == ui.metadataTab) {
		ui.metadataBrowser->clear();
		m_poliqarp->fetchMetadata(ui.textResultTable->currentIndex().row());
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

void PoliqarpWidget::updateTextQueries()
{
	int oldCount = ui.textResultTable->rowCount();
	ui.textResultTable->setRowCount(m_poliqarp->queryCount());

	bool extraColumn = false;

	QFont font = ui.textResultTable->font();
	font.setFamily(QSettings().value("Display/font", font.family()).toString());
	QFont boldFont = font;
	boldFont.setBold(true);

	for (int i = oldCount; i < m_poliqarp->queryCount(); i++) {
		DjVuLink item = m_poliqarp->query(i);
		if (item.columns() == 4)
			extraColumn = true;

		QTableWidgetItem* left = new QTableWidgetItem(item.leftContext());
		left->setFont(font);
		left->setTextAlignment(Qt::AlignRight);
		ui.textResultTable->setItem(i, 0, left);

		QTableWidgetItem* match = new QTableWidgetItem(item.match());
		match->setTextAlignment(Qt::AlignCenter);
		match->setFont(boldFont);
		match->setForeground(Qt::darkBlue);
		ui.textResultTable->setItem(i, 1, match);

		QTableWidgetItem* extraMatch = new QTableWidgetItem(item.rightMatch());
		extraMatch->setTextAlignment(Qt::AlignCenter);
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
	if (ui.textResultTable->isVisible())
		adjustTextColumns();
}

void PoliqarpWidget::updateGraphicalQueries()
{
	// Graphical results
	int oldCount = ui.graphicalResultList->count();
	for (int i = oldCount; i < m_poliqarp->queryCount(); i++)
		ui.graphicalResultList->addItem(m_poliqarp->query(i));
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
		QString family = settings.value("Display/font").toString();
		for (int r = 0; r < ui.textResultTable->rowCount(); r++)
			for (int c = 0; c < ui.textResultTable->columnCount(); c++)
				if (QTableWidgetItem* item = ui.textResultTable->item(r, c)) {
					QFont font = item->font();
					font.setFamily(family);
					item->setFont(font);
				}
	}
}

void PoliqarpWidget::configureServer()
{
	PoliqarpSettingsDialog dlg(this);
	dlg.restoreSettings(m_poliqarp->corpusUrl());
	if (dlg.exec()) {
		dlg.saveSettings();
		m_poliqarp->updateSettings();
	}
}

void PoliqarpWidget::adjustTextColumns()
{
	// Resize columns
	QHeaderView* header = ui.textResultTable->horizontalHeader();
	ui.textResultTable->resizeColumnToContents(1);
	int sizeUsed = header->sectionSize(1);
	if (!header->isSectionHidden(2)) {
		ui.textResultTable->resizeColumnToContents(2);
		sizeUsed += header->sectionSize(2);
	}

	int sizeLeft = qMax(header->width() - sizeUsed - 40, 100);
	header->resizeSection(0, sizeLeft / 2 - 5);
	header->resizeSection(3, sizeLeft / 2 - 5);
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



