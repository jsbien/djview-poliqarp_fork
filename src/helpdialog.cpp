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

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);

	QString language = QSettings().value("Display/language", QLocale::system().name().left(2))
							 .toString();
	QString content = readFileContent(language);
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
