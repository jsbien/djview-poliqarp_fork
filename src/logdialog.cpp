/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#include "logdialog.h"
#include <QtWidgets>

LogDialog::LogDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.clearButton, SIGNAL(clicked()), ui.logBrowser, SLOT(clear()));
	connect(ui.copyButton, SIGNAL(clicked()), this, SLOT(copy()));
	connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	QSettings settings;
	resize(settings.value("LogViewer/size", size()).toSize());
}

void LogDialog::setText(const QString& text)
{
	ui.logBrowser->setPlainText(text);
}

void LogDialog::hideEvent(QHideEvent* event)
{
	QSettings settings;
	settings.setValue("LogViewer/size", size());
	QDialog::hideEvent(event);
}

void LogDialog::copy()
{
	QApplication::clipboard()->setText(ui.logBrowser->toPlainText());
}
