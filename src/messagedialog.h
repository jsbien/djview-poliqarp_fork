/****************************************************************************
*   Copyright (C) 2012-2013 by Michal Rudolf
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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

class MessageDialog : public QObject
{
	Q_OBJECT
public:
	static void warning(const QString& message, const QString& caption = tr("Error"));
	static void error(const QString& message, const QString& caption = tr("Error"));
	static void information(const QString& message, const QString& caption = tr("Information"));
	static bool yesNoQuestion(const QString& message, const QString& caption = tr("Question"));
	static QMessageBox::StandardButton yesNoCancel(const QString& message,
																  const QString& caption = tr("Question"));
	static bool question(const QString& message, const QString& yes,	const QString& no,
								const QString& caption = tr("Question"));
	/** Select file to open. If @p category is set, use it to store/restore
	  default directory. */
	static QString openFile(const QString& pattern, const QString& message = tr("Open file"),
									const QString& category = "", const QString& defaultDir = "");
	/** Select executable file to open. If @p category is set, use it to store/restore
	  default directory. */
	static QString selectExecutable(const QString& message = tr("Select program"),
									const QString& category = QString());
	/** Select file to save. If @p category is set, use it to store/restore
	  default directory. */
	static QString saveFile(const QString& pattern,
									const QString& message = tr("Save file as"),
									const QString& category = "",
									const QString& defaultDir = "");
	/** Select file to save. If @p category is set, use it to store/restore
	  default directory. */
	static QString selectDirectory(const QString& message = tr("Select directory"),
											 const QString& category = QString());
	/** Save given image to file. */
	static QString saveImage(const QImage &image, const QString &pattern = tr("JPG files (*.jpg);;PNG files (*.png)"),
							const QString &message = tr("Save image"), const QString &category = QString());
};

#endif // MESSAGEBOX_H
