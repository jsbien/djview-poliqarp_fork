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

#include "entryindexdialog.h"
#include "helpdialog.h"
#include "logdialog.h"
#include "mainwindow.h"
#include "messagedialog.h"
#include "qdjvu.h"
#include "qdjvunet.h"
#include "djvulink.h"
#include "preferencesdialog.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	m_helpDialog = new HelpDialog(this);

	ui.setupUi(this);
	ui.mainToolBar->hide();

	connect(ui.poliqarpWidget, &PoliqarpWidget::documentRequested, ui.djvuWidget, &DjVuWidget::openLink);
	connect(ui.poliqarpWidget, &PoliqarpWidget::corpusSelected, this, &MainWindow::setSource);
	connect(ui.poliqarpWidget, &PoliqarpWidget::informationReceived, this, &MainWindow::showInformation);
	connect(ui.poliqarpWidget, &PoliqarpWidget::statusMessage, this, &MainWindow::showMessage);

	connect(ui.djvuWidget, &DjVuWidget::loading, this, &MainWindow::documentLoading);
	connect(ui.djvuWidget, &DjVuWidget::loaded, this, &MainWindow::documentLoaded);

	connect(ui.indexWidget, &IndexWidget::documentRequested, ui.djvuWidget, &DjVuWidget::openLink);

	setupActions();
	setWindowTitle(applicationName());
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

QString MainWindow::applicationName() const
{
	return tr("DjView for Poliqarp");
}

void MainWindow::restoreSettings()
{
	ui.indexWidget->hide();
	QSettings settings;
	settings.beginGroup("MainWindow");
	resize(settings.value("size", size()).toSize());
	ui.mainSplitter->restoreState(settings.value("mainWindowSplitter").toByteArray());
	ui.actionViewSidebar->setChecked(settings.value("poliqarpSidebar", true).toBool());
	settings.endGroup();

	QString welcome = settings.value("Help/welcome").toString();
	ui.actionWelcome->setVisible(!welcome.isEmpty());
}

void MainWindow::saveSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("mainWindowSplit", ui.mainSplitter->saveState());
	settings.setValue("poliqarpSidebar", ui.actionViewSidebar->isChecked());
	settings.endGroup();
}

void MainWindow::closeDocument()
{
	ui.stackWidget->setCurrentWidget(ui.corpusBrowser);
	ui.djvuWidget->closeDocument();
	statusBar()->clearMessage();
}

void MainWindow::exportResults()
{
	if (m_exportFilename.isEmpty())
		exportResultsAs();
	else ui.poliqarpWidget->exportResults(m_exportFilename);
}

void MainWindow::exportResultsAs()
{
	QString filename = MessageDialog::saveFile(tr("*.csv"));
	if (!filename.isEmpty())
		ui.poliqarpWidget->exportResults(m_exportFilename = filename);
}

void MainWindow::documentLoading(const DjVuLink& link)
{
	ui.stackWidget->setCurrentWidget(ui.djvuWidget);
	statusBar()->showMessage(tr("Loading %1...")
									 .arg(link.documentPath()));
}

void MainWindow::documentLoaded(const DjVuLink& link)
{
	ui.stackWidget->setCurrentWidget(ui.djvuWidget);
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
	 QString about = tr("%1\nVersion %2 %3\n(c) Michal Rudolf 2012-2014")
						 .arg(applicationName()).arg(Version::versionText()).arg(build);
	QMessageBox::about(this, tr("About application"), about);
}

void MainWindow::showWelcomeDocument()
{
	QString welcome = QSettings().value("Help/welcome").toString();
	if (!welcome.isEmpty()) {
		ui.djvuWidget->openFile(welcome);
		ui.stackWidget->setCurrentWidget(ui.djvuWidget);
	}
}



void MainWindow::setupActions()
{
	connect(ui.actionQuit, &QAction::triggered, this, &MainWindow::close);

	// File menu
	connect(ui.actionConfigure, &QAction::triggered, this, &MainWindow::configure);
	connect(ui.actionExportResults, &QAction::triggered, this, &MainWindow::exportResults);
	connect(ui.actionExportResultsAs, &QAction::triggered, this, &MainWindow::exportResultsAs);

	// View menu
	connect(ui.actionViewContinuous, &QAction::toggled, ui.djvuWidget,
			  &QDjVuWidget::setContinuous);
	connect(ui.actionViewSideBySide, &QAction::toggled, ui.djvuWidget,
			  &QDjVuWidget::setSideBySide);

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
	connect(ui.menuZoom, &QMenu::triggered, this, &MainWindow::zoomAction);

	// Rotation submenu
	ui.actionRotateLeft->setData("left");
	ui.actionRotateRight->setData("right");
	ui.actionRotate0->setData(0);
	ui.actionRotate90->setData(1);
	ui.actionRotate180->setData(2);
	ui.actionRotate270->setData(3);
	connect(ui.menuRotate, &QMenu::triggered, this, &MainWindow::rotateAction);

	// Sidebars
	connect(ui.actionViewSidebar, &QAction::toggled, ui.poliqarpWidget, &PoliqarpWidget::setVisible);
	connect(ui.actionViewIndex, &QAction::toggled, ui.indexWidget, &IndexWidget::setVisible);
	connect(ui.actionViewIndex, &QAction::toggled, this, &MainWindow::toggleIndexActions);

	// Go menu
	connect(ui.actionGoToFirstPage, &QAction::triggered, ui.djvuWidget, &QDjVuWidget::firstPage);
	connect(ui.actionGoToLastPage, &QAction::triggered, ui.djvuWidget, &QDjVuWidget::lastPage);
	connect(ui.actionGoToNextPage, &QAction::triggered, ui.djvuWidget, &QDjVuWidget::nextPage);
	connect(ui.actionGoToPreviousPage, &QAction::triggered, ui.djvuWidget, &QDjVuWidget::prevPage);
	connect(ui.actionScrollForward, &QAction::triggered, ui.djvuWidget, &QDjVuWidget::readNext);
	connect(ui.actionScrollBackward, &QAction::triggered, ui.djvuWidget, &QDjVuWidget::readPrev);

	// Index menu
	connect(ui.actionIndexOpen, &QAction::triggered, this, &MainWindow::indexOpen);
	connect(ui.actionIndexClose, &QAction::triggered, ui.indexWidget, &IndexWidget::close);
	connect(ui.actionIndexSave, &QAction::triggered, ui.indexWidget, &IndexWidget::save);


	// Help menu
	connect(ui.actionHelp, SIGNAL(toggled(bool)), this, SLOT(toggleHelp()));
	connect(ui.actionHelpAbout, SIGNAL(triggered()), this,
			  SLOT(showAboutDialog()));
	connect(ui.actionWelcome, SIGNAL(triggered()), this, SLOT(showWelcomeDocument()));
	connect(ui.actionShowLogs, SIGNAL(triggered()), this, SLOT(showLogs()));

	// File index actions
	connect(ui.actionAddEntry, SIGNAL(triggered()), this, SLOT(addIndexEntry()));
	connect(ui.actionUpdateEntry, SIGNAL(triggered()), this, SLOT(updateIndexEntry()));
	ui.djvuWidget->addCustomAction(ui.actionAddEntry);
	ui.djvuWidget->addCustomAction(ui.actionUpdateEntry);
}

void MainWindow::configure()
{
	PreferencesDialog dlg(this);
	if (dlg.exec()) {
		dlg.saveSettings();
		ui.poliqarpWidget->configure();
		ui.indexWidget->configure();

		QString welcome = QSettings().value("Help/welcome").toString();
		ui.actionWelcome->setVisible(!welcome.isEmpty());
	}
}

void MainWindow::setSource(const QString &title)
{
	setWindowTitle(QString("%1 - %2").arg(title).arg(applicationName()));
}

void MainWindow::showInformation(const QString &info)
{
	ui.corpusBrowser->setHtml(info);
	ui.stackWidget->setCurrentWidget(ui.corpusBrowser);
}

void MainWindow::showMessage(const QString& message)
{
	statusBar()->showMessage(message, 10 * 1000);
}

void MainWindow::showLogs()
{
	LogDialog dlg(this);
	dlg.setText(ui.poliqarpWidget->logs().join("\n"));
	dlg.exec();
	if (dlg.isCleared())
		ui.poliqarpWidget->clearLog();
}

void MainWindow::toggleHelp()
{
	m_helpDialog->setVisible(ui.actionHelp->isChecked());
}

void MainWindow::indexOpen()
{
	QString filename = MessageDialog::openFile(tr("CSV files (*.csv)"), tr("Select index file"),
															 "Index");
	if (!filename.isEmpty())
		ui.actionViewIndex->setChecked(ui.indexWidget->open(filename));
}

void MainWindow::indexClose()
{
	ui.indexWidget->close();
	ui.actionViewIndex->setChecked(false);
}

void MainWindow::toggleIndexActions()
{
	ui.actionAddEntry->setVisible(ui.actionViewIndex->isChecked());
	ui.actionUpdateEntry->setVisible(ui.actionViewIndex->isChecked());
}

void MainWindow::addIndexEntry()
{
	QUrl url = ui.djvuWidget->lastSelection();
	if (url.isValid()) {
		QString hiddenText = ui.djvuWidget->getTextForRect(ui.djvuWidget->lastRegion());
		if (hiddenText.count() > 60) {
			int lastSpace = hiddenText.mid(50).indexOf(' ');
			if (lastSpace != -1)
				hiddenText.truncate(50 + lastSpace - 1);
		}
		Entry entry(hiddenText);
		entry.link = url;
		EntryIndexDialog dlg(this);
		dlg.setEntry(entry);
		if (dlg.exec())
			ui.indexWidget->addEntry(dlg.entry());
	}
}

void MainWindow::updateIndexEntry()
{
	QUrl url = ui.djvuWidget->lastSelection();
	if (url.isValid())
		ui.indexWidget->updateCurrentEntry(url);
}



