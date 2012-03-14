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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "recentfiles.h"
#include "djvulink.h"

class QDjVuContext;
class QDjVuDocument;

class MainWindow : public QMainWindow
{
	 Q_OBJECT

public:
	 explicit MainWindow(QWidget *parent = 0);

private slots:
	/** Open given file. */
	void open(const QString& filename);
	/** Select file and open it. */
	void selectFileToOpen();
	/** Select file and open it. */
	void selectUrlToOpen();
	/** Show about information. */
	void showAboutDialog();
	/** Handle zoom action. */
	void zoomAction(QAction* data);
	/** Handle rotate action. */
	void rotateAction(QAction* data);
	/** Open document. */
	void openDocument(const QUrl& url);
	/** Open document. */
	void openDocument(const DjVuLink& link);
	/** Page info. */
	void pageLoaded();
	/** Configure. */
	void configure();

protected:
	/** Confirm quitting. */
	void closeEvent(QCloseEvent* event);

	/** Connect actions. */
	void setupActions();
	/** Read configuration. */
	void restoreSettings();
	/** Save settings. */
	void saveSettings();
	/** Check if the data is saved. */
	bool queryClose();

	Ui::MainWindow ui;
	RecentFiles m_recentFiles;
	QDjVuContext* m_context;
	QDjVuDocument* m_document;
	DjVuLink m_currentLink;
	static const QString m_applicationName;
};

#endif // MAINWINDOW_H

