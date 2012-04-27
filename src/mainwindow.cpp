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
#include "djvulink.h"
#include "preferencesdialog.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.poliqarpWidget, SIGNAL(documentRequested(DjVuLink)), ui.djvuWidget,
			  SLOT(openLink(DjVuLink)));
	connect(ui.poliqarpWidget, SIGNAL(sourceUpdated(QString)), ui.corpusBrowser,
			  SLOT(setHtml(QString)));
	connect(ui.poliqarpWidget, SIGNAL(sourceNameChanged(QString)), this,
			  SLOT(setSourceName(QString)));

	connect(ui.djvuWidget, SIGNAL(loading(DjVuLink)), this,
			  SLOT(documentLoading(DjVuLink)));
	connect(ui.djvuWidget, SIGNAL(loaded(DjVuLink)), this,
			  SLOT(documentLoaded(DjVuLink)));

	setupActions();
	setWindowTitle(m_applicationName);
	show();
	restoreSettings();

	ui.poliqarpWidget->connectToServer();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (queryClose()) {
		saveSettings();
		closeDocument();
		ui.poliqarpWidget->clear();
	}
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

void MainWindow::closeDocument()
{
	ui.stackWidget->setCurrentWidget(ui.corpusBrowser);
	ui.djvuWidget->closeDocument();
	statusBar()->clearMessage();
}

void MainWindow::documentLoading(const DjVuLink& link)
{
	statusBar()->showMessage(tr("Loading %1...")
									 .arg(link.documentPath()));
}

void MainWindow::documentLoaded(const DjVuLink& link)
{
	statusBar()->showMessage(tr("%1: page %2")
									 .arg(link.documentPath())
									 .arg(link.page() + 1));
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
	QString build = Version::buildNumber() ? tr(" (build %1)")
														  .arg(Version::buildText()) : "";
	QString about = tr("%1\nVersion %2 %3\n(c) Michal Rudolf 2012")
					.arg(m_applicationName).arg(Version::versionText()).arg(build);
	QMessageBox::about(this, tr("About application"), about);
}





void MainWindow::setupActions()
{
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

	// Settings menu
	connect(ui.actionConfigure, SIGNAL(triggered()), this, SLOT(configure()));
}

void MainWindow::configure()
{
	PreferencesDialog dlg(this);
	if (dlg.exec())
		dlg.saveSettings();
}

void MainWindow::setSourceName(const QString &title)
{
	setWindowTitle(QString("%1 - %2").arg(title).arg(m_applicationName));
}

const QString MainWindow::m_applicationName = QT_TR_NOOP("DjView-Poliqarp");



