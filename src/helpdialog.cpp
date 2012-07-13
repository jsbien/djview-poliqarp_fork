/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtGui>
#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);

	QString content = readFileContent(QLocale::system().name().left(2));
	if (content.isEmpty())
		content = readFileContent("en");
	ui.helpBrowser->setText(content);
}

QString HelpDialog::readFileContent(const QString &language) const
{
	QFile file(QString(":help/%1.html").arg(language));
	if (!file.open(QIODevice::ReadOnly))
		return "";
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	return stream.readAll();
}
