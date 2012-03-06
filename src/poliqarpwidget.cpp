/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "poliqarpwidget.h"
#include "poliqarp.h"
#include "messagedialog.h"


PoliqarpWidget::PoliqarpWidget(QWidget *parent) :
	 QWidget(parent)
{
	 ui.setupUi(this);
	 ui.resultTableWidget->verticalHeader()->setDefaultSectionSize(
				 1.2 * ui.resultTableWidget->verticalHeader()->fontMetrics().height());

	 connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
	 connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
	 connect(ui.corpusCombo, SIGNAL(currentIndexChanged(int)), this,
				SLOT(doSelectSource()));
	 connect(ui.resultTableWidget, SIGNAL(doubleClicked(QModelIndex)), this,
				SLOT(showDocument()));

	 m_poliqarp = new Poliqarp(this);
	 connect(m_poliqarp, SIGNAL(connected(QStringList)), this,
				SLOT(connected(QStringList)));
	 connect(m_poliqarp, SIGNAL(connectionError(QString)), this,
				SLOT(connectionError(QString)));
	 connect(m_poliqarp, SIGNAL(sourceSelected()), this,
				SLOT(sourceSelected()));
	 connect(m_poliqarp, SIGNAL(queryFinished()), this,
				SLOT(updateQueries()));
}


void PoliqarpWidget::connectToServer()
{
	QUrl url;
	url.setHost(ui.urlCombo->currentText());
	url.setScheme("http");
	if (!url.isValid()) {
		MessageDialog::warning("This URL is not valid");
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
	m_poliqarp->query(ui.queryEdit->text());
}

void PoliqarpWidget::doSelectSource()
{
	ui.searchButton->setEnabled(false);
	ui.resultTableWidget->setRowCount(0);
	m_poliqarp->setCurrentSource(ui.corpusCombo->currentIndex());
}

void PoliqarpWidget::connected(const QStringList& sources)
{
	unsetCursor();
	ui.corpusCombo->addItems(sources);
	ui.connectButton->setEnabled(true);
	ui.corpusCombo->setEnabled(true);
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
	ui.queryEdit->setFocus();
}

void PoliqarpWidget::updateQueries()
{
	unsetCursor();
	ui.resultTableWidget->setRowCount(qMax(m_poliqarp->queryCount(), 1));
	QFont boldFont = ui.resultTableWidget->font();
	boldFont.setBold(true);
	for (int i = 0; i < m_poliqarp->queryCount(); i++) {
		DjVuLink item = m_poliqarp->query(i);
		QTableWidgetItem* left = new QTableWidgetItem(item.leftContext());
		left->setTextAlignment(Qt::AlignRight);
		ui.resultTableWidget->setItem(i, 0, left);

		QTableWidgetItem* center = new QTableWidgetItem(item.word());
		center->setTextAlignment(Qt::AlignCenter);
		center->setFont(boldFont);
		center->setForeground(Qt::darkBlue);
		ui.resultTableWidget->setItem(i, 1, center);

		QTableWidgetItem* right = new QTableWidgetItem(item.rightContext());
		right->setTextAlignment(Qt::AlignLeft);
		ui.resultTableWidget->setItem(i, 2, right);
	}
	if (m_poliqarp->queryCount() == 0) {
		QFont italicFont = ui.resultTableWidget->font();
		italicFont.setItalic(true);
		QTableWidgetItem* center = new QTableWidgetItem(tr("No matches"));
		center->setTextAlignment(Qt::AlignCenter);
		center->setFont(italicFont);
		ui.resultTableWidget->setItem(0, 0, new QTableWidgetItem);
		ui.resultTableWidget->setItem(0, 1, center);
		ui.resultTableWidget->setItem(0, 2, new QTableWidgetItem);
	}

	// Resize columns
	QHeaderView* header = ui.resultTableWidget->horizontalHeader();
	ui.resultTableWidget->resizeColumnToContents(1);
	int sizeLeft = header->width() - header->sectionSize(1) - 20;
	header->resizeSection(0, sizeLeft / 2 - 5);
	header->resizeSection(2, sizeLeft / 2 - 5);
}

void PoliqarpWidget::showDocument()
{
	if (ui.resultTableWidget->currentRow() != -1) {
		DjVuLink item = m_poliqarp->query(ui.resultTableWidget->currentRow());
		if (item.link().isValid())
			emit documentRequested(item);
	}
}



