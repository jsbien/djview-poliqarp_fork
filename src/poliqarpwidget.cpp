/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "poliqarpwidget.h"
#include "poliqarp.h"
#include "messagedialog.h"
#include "djvuitemlist.h"


PoliqarpWidget::PoliqarpWidget(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);
	ui.textResultTable->verticalHeader()->setDefaultSectionSize(
				1.2 * ui.textResultTable->verticalHeader()->fontMetrics().height());

	connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
	connect(ui.corpusCombo, SIGNAL(currentIndexChanged(int)), this,
			  SLOT(doSelectSource()));
	connect(ui.textResultTable, SIGNAL(doubleClicked(QModelIndex)), this,
			  SLOT(showDocument(QModelIndex)));
//	connect(ui.graphicalResultList, SIGNAL(doubleClicked(QModelIndex)), this,
//			  SLOT(showDocument(QModelIndex)));
	connect(ui.queryCombo->lineEdit(), SIGNAL(returnPressed()), this,
			  SLOT(doSearch()));

	m_poliqarp = new Poliqarp(this);
	connect(m_poliqarp, SIGNAL(connected(QStringList)), this,
			  SLOT(connected(QStringList)));
	connect(m_poliqarp, SIGNAL(connectionError(QString)), this,
			  SLOT(connectionError(QString)));
	connect(m_poliqarp, SIGNAL(sourceSelected()), this,
			  SLOT(sourceSelected()));
	connect(m_poliqarp, SIGNAL(queryFinished()), this,
			  SLOT(updateQueries()));

	connect(ui.nextButton, SIGNAL(clicked()), m_poliqarp, SLOT(nextQuery()));
	connect(ui.previousButton, SIGNAL(clicked()), m_poliqarp, SLOT(previousQuery()));

	QSettings settings;
	settings.beginGroup("Poliqarp");
	ui.queryCombo->addItems(settings.value("queries").toStringList());
	ui.queryCombo->clearEditText();
	settings.endGroup();

}


PoliqarpWidget::~PoliqarpWidget()
{
	QSettings settings;
	settings.beginGroup("Poliqarp");
	QStringList items;
	for (int i = 0; i < ui.queryCombo->count(); i++)
		items.append(ui.queryCombo->itemText(i));
	settings.setValue("queries", items);
	settings.endGroup();
}

void PoliqarpWidget::connectToServer()
{
	QUrl url;
	url.setHost(ui.urlCombo->currentText());

	// MRTODO: remove when Kanji is ported
	if (url.host().contains("wbl"))
		url.setPath("/en");
	url.setScheme("http");
	if (!url.isValid()) {
		MessageDialog::warning(tr("This URL is not valid"));
		return;
	}
	ui.corpusCombo->clear();
	ui.connectButton->setEnabled(false);
	setCursor(QCursor(Qt::WaitCursor));
	m_poliqarp->connectToServer(url);
}

void PoliqarpWidget::doSearch()
{
	setCursor(QCursor(Qt::WaitCursor));
	m_poliqarp->query(ui.queryCombo->currentText());
}

void PoliqarpWidget::doSelectSource()
{
	ui.searchButton->setEnabled(false);
	ui.textResultTable->setRowCount(0);
	m_poliqarp->setCurrentSource(ui.corpusCombo->currentIndex());
}

void PoliqarpWidget::connected(const QStringList& sources)
{
	unsetCursor();
	ui.corpusCombo->addItems(sources);
	ui.connectButton->setEnabled(true);
	ui.corpusCombo->setEnabled(true);
	QSettings settings;
	int lastIndex = settings.value(QString("Poliqarp/") +
											 m_poliqarp->serverUrl().host(), 0).toInt();
	ui.corpusCombo->setCurrentIndex(lastIndex);
}

void PoliqarpWidget::connectionError(const QString &message)
{
	unsetCursor();
	ui.connectButton->setEnabled(true);
	MessageDialog::warning(message);
}

void PoliqarpWidget::sourceSelected()
{
	ui.searchButton->setEnabled(true);
	ui.queryCombo->setFocus();
	QSettings settings;
	settings.setValue(QString("Poliqarp/") + m_poliqarp->serverUrl().host(),
							ui.corpusCombo->currentIndex());
}

void PoliqarpWidget::updateQueries()
{
	unsetCursor();
	updateTextQueries();
	updateGraphicalQueries();

	ui.previousButton->setEnabled(m_poliqarp->hasPreviousQueries());
	ui.nextButton->setEnabled(m_poliqarp->hasNextQueries());
}

void PoliqarpWidget::showDocument(const QModelIndex& index)
{
	if (index.isValid()) {
		DjVuLink item = m_poliqarp->query(index.row());
		if (item.link().isValid())
			emit documentRequested(item);
	}
}

void PoliqarpWidget::updateTextQueries()
{
	ui.textResultTable->setRowCount(m_poliqarp->queryCount());
	QFont boldFont = ui.textResultTable->font();
	boldFont.setBold(true);
	for (int i = 0; i < m_poliqarp->queryCount(); i++) {
		DjVuLink item = m_poliqarp->query(i);
		QTableWidgetItem* left = new QTableWidgetItem(item.leftContext());
		left->setTextAlignment(Qt::AlignRight);
		ui.textResultTable->setItem(i, 0, left);

		QTableWidgetItem* center = new QTableWidgetItem(item.word());
		center->setTextAlignment(Qt::AlignCenter);
		center->setFont(boldFont);
		center->setForeground(Qt::darkBlue);
		ui.textResultTable->setItem(i, 1, center);

		QTableWidgetItem* right = new QTableWidgetItem(item.rightContext());
		right->setTextAlignment(Qt::AlignLeft);
		ui.textResultTable->setItem(i, 2, right);
	}
	if (m_poliqarp->queryCount() == 0)
		ui.matchLabel->setText(tr("No matches"));
	else {
		QString text = ui.queryCombo->currentText();
		int old = ui.queryCombo->findText(text);
		if (old != -1)
			ui.queryCombo->removeItem(old);
		else if (ui.queryCombo->count() >= 20)
			ui.queryCombo->removeItem(ui.queryCombo->count() - 1);
		ui.queryCombo->insertItem(0, text);
		ui.queryCombo->setCurrentIndex(0);
		ui.matchLabel->setText(tr("Matches %1-%2 of %3")
									  .arg(m_poliqarp->firstMatchIndex() + 1)
									  .arg(m_poliqarp->firstMatchIndex() +
											 m_poliqarp->queryCount())
									  .arg(m_poliqarp->matchesFound()));
	}


	// Resize columns
	QHeaderView* header = ui.textResultTable->horizontalHeader();
	ui.textResultTable->resizeColumnToContents(1);
	int sizeLeft = header->width() - header->sectionSize(1) - 20;
	header->resizeSection(0, sizeLeft / 2 - 5);
	header->resizeSection(2, sizeLeft / 2 - 5);
}

void PoliqarpWidget::updateGraphicalQueries()
{
	// Graphical results
	ui.graphicalResultList->clear();
	for (int i = 0; i < m_poliqarp->queryCount(); i++)
		ui.graphicalResultList->addItem(m_poliqarp->query(i));
}



