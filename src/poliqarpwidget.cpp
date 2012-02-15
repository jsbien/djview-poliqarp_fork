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

	 connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(doConnect()));
	 connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
	 connect(ui.corpusCombo, SIGNAL(currentIndexChanged(int)), this,
				SLOT(doSelectSource()));

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


void PoliqarpWidget::doConnect()
{
	QUrl url = ui.ulrEdit->text();
	if (!url.isValid() || !url.scheme().startsWith("http")) {
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
	m_poliqarp->query(ui.queryEdit->text());
}

void PoliqarpWidget::doSelectSource()
{
	ui.searchButton->setEnabled(false);
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
}

void PoliqarpWidget::updateQueries()
{
	ui.resultTableWidget->setRowCount(m_poliqarp->queryCount());
	QFont boldFont = ui.resultTableWidget->font();
	boldFont.setBold(true);
	for (int i = 0; i < m_poliqarp->queryCount(); i++) {
		QueryItem item = m_poliqarp->query(i);
		QTableWidgetItem* left = new QTableWidgetItem(item.leftContext());
		left->setTextAlignment(Qt::AlignRight);
		ui.resultTableWidget->setItem(i, 0, left);

		QTableWidgetItem* center = new QTableWidgetItem(item.word());
		center->setTextAlignment(Qt::AlignCenter);
		center->setFont(boldFont);
		center->setForeground(Qt::darkBlue);
		ui.resultTableWidget->setItem(i, 1, center);

		QTableWidgetItem* right = new QTableWidgetItem(item.leftContext());
		right->setTextAlignment(Qt::AlignRight);
		ui.resultTableWidget->setItem(i, 2, right);

	}
}



