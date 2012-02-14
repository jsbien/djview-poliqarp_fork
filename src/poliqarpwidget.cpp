/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtXml>
#include "poliqarpwidget.h"
#include "messagedialog.h"

PoliqarpWidget::PoliqarpWidget(QWidget *parent) :
	 QWidget(parent)
{
	 ui.setupUi(this);
	 m_network = new QNetworkAccessManager(this);
	 connect(m_network, SIGNAL(finished(QNetworkReply*)), this,
				SLOT(replyFinished(QNetworkReply*)));
	 connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(doConnect()));
	 connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
}

void PoliqarpWidget::doConnect()
{
	QUrl url = ui.ulrEdit->text();
	if (!url.isValid() || !url.scheme().startsWith("http")) {
		MessageDialog::warning("This URL is not valid");
		return;
	}
	m_network->get(QNetworkRequest(url));
	ui.connectButton->setEnabled(false);
	ui.corpusCombo->clear();
	setCursor(QCursor(Qt::WaitCursor));
}

void PoliqarpWidget::doSearch()
{
}

void PoliqarpWidget::replyFinished(QNetworkReply *reply)
{
	ui.connectButton->setEnabled(true);
	unsetCursor();
	reply->deleteLater();

	if (reply->error())
		MessageDialog::warning("Could not connect to the server.\nPlease check the URL.");
	else if (!parseSources(reply))
		MessageDialog::warning(tr("This does not look like a Poliqarp server."));
	else {
		ui.corpusCombo->setEnabled(true);
		ui.queryEdit->setEnabled(true);
	}
}

bool PoliqarpWidget::parseSources(QIODevice* device)
{
	QDomDocument document;
	if (!document.setContent(device, false))
		return false;

//	QDomElement title = document.elementsByTagName("title").at(0).toElement();
//	if (title.firstChild().toText().nodeValue() != "Wyszukiwarka Poliqarp")
//		return false;

	QDomElement list = document.elementsByTagName("ul").at(1).toElement();
	if (list.isNull())
		return false;

	QDomNodeList sources = list.elementsByTagName("a");
	for (int i = 0; i < sources.count(); i++) {
		QDomElement element = sources.at(i).toElement();
		if (element.isNull())
			continue;
		QString tag = element.attribute("href");
		QString label = element.firstChild().toText().nodeValue();
		if (!tag.isEmpty() && !label.isEmpty())
			ui.corpusCombo->addItem(label, tag);
	}
	return ui.corpusCombo->count() > 0;
}

