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



