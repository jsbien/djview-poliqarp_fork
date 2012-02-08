/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf
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

#include "messagedialog.h"

void MessageDialog::warning(const QString &message, const QString &caption)
{
	QMessageBox::warning(QApplication::activeWindow(), caption, message);
}

void MessageDialog::error(const QString &message, const QString &caption)
{
	QMessageBox::critical(QApplication::activeWindow(), caption, message);
}

void MessageDialog::information(const QString &message, const QString &caption)
{
	QMessageBox::information(QApplication::activeWindow(), caption, message);
}

bool MessageDialog::yesNoQuestion(const QString &message, const QString &caption)
{
	return QMessageBox::question(QApplication::activeWindow(), caption,
										  message, QMessageBox::Yes | QMessageBox::No)
	== QMessageBox::Yes;
}

QMessageBox::StandardButton MessageDialog::yesNoCancel(const QString &message,
																	 const QString &caption)
{
	return QMessageBox::question(QApplication::activeWindow(), caption,
										  message, QMessageBox::Yes | QMessageBox::No |
										  QMessageBox::Cancel);
}

bool MessageDialog::question(const QString &message, const QString &yes, const QString &no, const QString &caption)
{
	QMessageBox box(QApplication::activeWindow());
	box.setIcon(QMessageBox::Question);
	box.setWindowTitle(caption);
	box.setText(message);
	QPushButton* yesButton = box.addButton(QMessageBox::Yes);
	yesButton->setText(yes);
	box.setDefaultButton(yesButton);
	QPushButton* noButton = box.addButton(QMessageBox::No);
	noButton->setText(no);
	box.setEscapeButton(noButton);
	box.exec();
	return box.clickedButton() == yesButton;
}

QString MessageDialog::openFile(const QString& pattern, const QString& message,
												const QString& category)
{
	QSettings settings;

	QFileDialog dlg(QApplication::activeWindow());
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setNameFilter(pattern + tr(";;All files (*.*)"));
	dlg.setWindowTitle(message);
	dlg.setDirectory(settings.value(QString("Paths/") + category,
											  QDir::homePath()).toString());
	if (!dlg.exec())
		return QString();
	if (!category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								dlg.directory().absolutePath());
	return dlg.selectedFiles().first();
}

QString MessageDialog::saveFile(const QString &pattern, const QString &message,
										  const QString &category)
{
	QSettings settings;

	QFileDialog dlg(QApplication::activeWindow());
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	dlg.setFileMode(QFileDialog::AnyFile);
	dlg.setNameFilter(pattern + tr(";;All files (*.*)"));
	dlg.setWindowTitle(message);
	dlg.setDirectory(settings.value(QString("Paths/") + category,
											  QDir::homePath()).toString());
	QRegExp exp("\\.[a-zA-Z]*");
	if (exp.indexIn(dlg.selectedFilter()) != -1)
		dlg.setDefaultSuffix(exp.cap(0).mid(1));
	if (!dlg.exec())
		return QString();
	if (!category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								dlg.directory().absolutePath());
	return dlg.selectedFiles().first();
}

