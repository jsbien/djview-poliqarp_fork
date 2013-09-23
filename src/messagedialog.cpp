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
	box.adjustSize();
	box.exec();
	return box.clickedButton() == yesButton;
}

QString MessageDialog::openFile(const QString& pattern, const QString& message,
												const QString& category, const QString& defaultDir)
{
	QSettings settings;

	QString filePattern = pattern + tr(";;All files (*.*)");
	QString dir = defaultDir.isEmpty() ? QDir::homePath() : defaultDir;
	if (!category.isEmpty())
		dir = settings.value(QString("Paths/") + category, dir).toString();
	QString selected = QFileDialog::getOpenFileName(QApplication::activeWindow(), message,
										  dir, filePattern);
	if (!selected.isEmpty() && !category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								QFileInfo(selected).absolutePath());
	return selected;
}

QString MessageDialog::selectExecutable(const QString& message,
												const QString& category)
{
	QSettings settings;
	QString executableFilter = tr("Programs") + " (*.exe)";
#ifndef Q_OS_WIN32
	executableFilter.clear();
#endif

	QString dir;
	if (!category.isEmpty())
		dir = settings.value(QString("Paths/") + category, dir).toString();
	QString selected = QFileDialog::getOpenFileName(QApplication::activeWindow(), message,
										  dir, executableFilter);
	if (!selected.isEmpty() && !category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								QFileInfo(selected).absolutePath());
	return selected;
}

QString MessageDialog::saveFile(const QString &pattern, const QString &message,
										  const QString &category, const QString &defaultDir)
{
	QSettings settings;

	QString filePattern = pattern + tr(";;All files (*.*)");
	QString dir = defaultDir.isEmpty() ? QDir::homePath() : defaultDir;
	if (!category.isEmpty())
		dir = settings.value(QString("Paths/") + category, dir).toString();
	QString selected = QFileDialog::getSaveFileName(QApplication::activeWindow(), message,
										  dir, filePattern);


	if (!selected.isEmpty() && QFileInfo(selected).suffix().isEmpty()) {
		QRegExp exp("\\.[a-zA-Z]*");
		if (exp.indexIn(filePattern.section(";;", 0, 0)) != -1)
			selected.append(exp.cap(0));
	}
	if (!selected.isEmpty() && !category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								QFileInfo(selected).absolutePath());
	return selected;
}

QString MessageDialog::selectDirectory(const QString &message, const QString &category)
{
	QSettings settings;

	QString dir = QDir::homePath();
	if (!category.isEmpty())
		dir = settings.value(QString("Paths/") + category, dir).toString();
	QString selected = QFileDialog::getExistingDirectory(QApplication::activeWindow(), message,
										  dir);
	if (!selected.isEmpty() && !category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								QFileInfo(selected).absolutePath());
	return selected;

}

QString MessageDialog::saveImage(const QImage &image, const QString &pattern, const QString &message, const QString &category)
{
	QSettings settings;
	QString dir = QDir::homePath();
	if (!category.isEmpty())
		dir = settings.value(QString("Paths/") + category, dir).toString();
	QString selected = QFileDialog::getSaveFileName(QApplication::activeWindow(), message,
										  dir, pattern);


	if (!selected.isEmpty() && QFileInfo(selected).suffix().isEmpty()) {
		QRegExp exp("\\.[a-zA-Z]*");
		if (exp.indexIn(pattern.section(";;", 0, 0)) != -1)
			selected.append(exp.cap(0));
	}
	if (!selected.isEmpty() && !category.isEmpty())
		settings.setValue(QString("Paths/") + category,
								QFileInfo(selected).absolutePath());

	if (!image.save(selected))
		selected.clear();
	return selected;
}

