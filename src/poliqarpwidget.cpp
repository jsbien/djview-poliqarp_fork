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

	 m_poliqarp = new Poliqarp(this);
	 connect(m_poliqarp, SIGNAL(connected()), this, SLOT(connected()));
	 connect(m_poliqarp, SIGNAL(connectionError(QString)), this,
				SLOT(connectionError(QString)));
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
}

void PoliqarpWidget::connected()
{
	unsetCursor();
	foreach (const QString& item, m_poliqarp->sources())
		ui.corpusCombo->addItem(item.section('=', 0, 0), item.section('=', 1));
	ui.connectButton->setEnabled(true);
	ui.corpusCombo->setEnabled(true);
	ui.searchButton->setEnabled(true);
	ui.queryEdit->setEnabled(true);
}

void PoliqarpWidget::connectionError(const QString &message)
{
	unsetCursor();
	ui.connectButton->setEnabled(true);
	MessageDialog::warning(message);
}


