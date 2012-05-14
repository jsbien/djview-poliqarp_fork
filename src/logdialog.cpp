/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "logdialog.h"

LogDialog::LogDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}

void LogDialog::setText(const QString& text)
{
	ui.logBrowser->setPlainText(text);
}
