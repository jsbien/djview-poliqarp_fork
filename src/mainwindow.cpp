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

#include "mainwindow.h"
#include "messagedialog.h"
#include "qdjvu.h"
#include "qdjvuhttp.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_recentFiles(this)
{
	ui.setupUi(this);
	m_recentFiles.plug(ui.actionFileRecent);
	connect(&m_recentFiles, SIGNAL(selected(QString)), this, SLOT(open(QString)));

	m_context = new QDjVuContext(m_applicationName.toLatin1(), this);
	m_document = new QDjVuDocument(this);

	setupActions();
	setWindowTitle(QString("%1 - %2").arg(tr("[Untitled]"))
													  .arg(m_applicationName));
	show();
	restoreSettings();

	if (QApplication::instance()->argc() > 1)
		open(QApplication::instance()->argv()[1]);

}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (queryClose())
		saveSettings();
	else event->ignore();
}

bool MainWindow::queryClose()
{
	return true;
}

void MainWindow::restoreSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	resize(settings.value("size", size()).toSize());
	ui.mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());
	ui.actionViewSidebar->setChecked(settings.value("sidebar", true).toBool());
	settings.endGroup();
}

void MainWindow::saveSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("mainSplitter", ui.mainSplitter->saveState());
	settings.setValue("sidebar", ui.actionViewSidebar->isChecked());
	settings.endGroup();
}

void MainWindow::open(const QString &filename)
{
	/*** Open file. ***/
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	ui.djvuWidget->setDocument(0);
	if (m_document->setFileName(m_context, filename, true)) {
		ui.djvuWidget->setDocument(m_document);
		m_recentFiles.addFile(filename);
		setWindowTitle(QString("%1 - %2").arg(QFileInfo(filename).baseName())
							.arg(m_applicationName));
	}
	else MessageDialog::warning(tr("Cannot open file\n%1").arg(filename));
	QApplication::restoreOverrideCursor();
}

void MainWindow::selectFileToOpen()
{
	QString filename = MessageDialog::openFile(tr("DjVu files (*.djvu)"),
			tr("Open file"), "Files");
	if (!filename.isEmpty())
		open(filename);
}

void MainWindow::selectUrlToOpen()
{
	QString location = "http://";
	location = QInputDialog::getText(this,
												tr("Open Location"),
												tr("Enter the URL of a DjVu document:"),
												QLineEdit::Normal, location);
	if (!location.isEmpty()) {
		QUrl url(location);
		if (url.isValid()) {
			QDjVuHttpDocument* httpDocument = new QDjVuHttpDocument(this);
			ui.djvuWidget->setDocument(httpDocument);
			httpDocument->setUrl(m_context, url);
			m_document->deleteLater();
			m_document = httpDocument;
		}
	}
}

void MainWindow::zoomAction(QAction *action)
{
	if (action->data().toString() == "in")
		ui.djvuWidget->zoomIn();
	else if (action->data().toString() == "out")
		ui.djvuWidget->zoomOut();
	else ui.djvuWidget->setZoom(action->data().toInt());
}

void MainWindow::rotateAction(QAction *action)
{
	if (action->data().toString() == "left")
		ui.djvuWidget->rotateLeft();
	else if (action->data().toString() == "right")
		ui.djvuWidget->rotateRight();
	else ui.djvuWidget->setRotation(action->data().toInt());
}


void MainWindow::showAboutDialog()
{
	const QString VERSION = "0.1";
	QString about = tr("%1\n"
							 "Version %2\n(c) Michal Rudolf 2012")
			.arg(m_applicationName).arg(VERSION);
	QMessageBox::about(this, tr("About application"), about);
}





void MainWindow::setupActions()
{
	connect(ui.actionFileOpen, SIGNAL(triggered()), this,
			  SLOT(selectFileToOpen()));
	connect(ui.actionFileOpenLocation, SIGNAL(triggered()), this,
			  SLOT(selectUrlToOpen()));

	connect(ui.actionHelpAbout, SIGNAL(triggered()), this,
			  SLOT(showAboutDialog()));

	// View menu
	connect(ui.actionViewContinuous, SIGNAL(toggled(bool)), ui.djvuWidget,
			  SLOT(setContinuous(bool)));
	connect(ui.actionViewSideBySide, SIGNAL(toggled(bool)), ui.djvuWidget,
			  SLOT(setSideBySide(bool)));

	// Zoom submenu
	ui.actionZoomIn->setData("in");
	ui.actionZoomOut->setData("out");
	ui.actionZoomOneToOne->setData(QDjVuWidget::ZOOM_ONE2ONE);
	ui.actionZoomFitWidth->setData(QDjVuWidget::ZOOM_FITWIDTH);
	ui.actionZoomFitPage->setData(QDjVuWidget::ZOOM_FITPAGE);
	ui.actionZoom300->setData(300);
	ui.actionZoom200->setData(200);
	ui.actionZoom150->setData(150);
	ui.actionZoom100->setData(100);
	ui.actionZoom75->setData(75);
	ui.actionZoom50->setData(50);
	connect(ui.menuZoom, SIGNAL(triggered(QAction*)), this,
			  SLOT(zoomAction(QAction*)));

	// Rotation submenu
	ui.actionRotateLeft->setData("left");
	ui.actionRotateRight->setData("right");
	ui.actionRotate0->setData(0);
	ui.actionRotate90->setData(1);
	ui.actionRotate180->setData(2);
	ui.actionRotate270->setData(3);
	connect(ui.menuRotate, SIGNAL(triggered(QAction*)), this,
			  SLOT(rotateAction(QAction*)));

	// Go menu
	connect(ui.actionGoToFirstPage, SIGNAL(triggered()), ui.djvuWidget,
			  SLOT(firstPage()));
	connect(ui.actionGoToLastPage, SIGNAL(triggered()), ui.djvuWidget,
			  SLOT(lastPage()));
	connect(ui.actionGoToNextPage, SIGNAL(triggered()), ui.djvuWidget,
			  SLOT(nextPage()));
	connect(ui.actionGoToPreviousPage, SIGNAL(triggered()), ui.djvuWidget,
			  SLOT(prevPage()));

}




const QString MainWindow::m_applicationName = QT_TR_NOOP("DjView-Poliqarp");





