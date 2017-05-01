/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf
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
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
	QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
#endif

	QApplication a(argc, argv);
	a.setOrganizationName("djview-poliqarp");
	a.setApplicationName("djview-poliqarp");

	QString language = QSettings().value("Display/language", QLocale::system().name().left(2))
							 .toString();

	QString translation = QString(":i18n/%1.qm").arg(language);

	QTranslator translator;
	if (translator.load(translation))
		a.installTranslator(&translator);

	QTranslator qtTranslator;
	if (qtTranslator.load(QString("/usr/share/qt4/translations/qt_%1.qm")
								 .arg(language)))
		a.installTranslator(&qtTranslator);

	MainWindow w;
	w.show();

	a.exec();
}
