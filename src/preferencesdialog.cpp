/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "messagedialog.h"
#include "preferencesdialog.h"
#include <QtGui>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
	 QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.highlightButton, SIGNAL(clicked()), this, SLOT(selectHighlightColor()));
	connect(ui.fontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
	connect(ui.addServerButton, SIGNAL(clicked()), this, SLOT(addServer()));
	connect(ui.removeServerButton, SIGNAL(clicked()), this, SLOT(removeServer()));

	ui.tabWidget->setCurrentWidget(ui.generalTab);

	ui.languageCombo->addItem(tr("[System]"));
	ui.languageCombo->addItem(tr("English"), "en");
	ui.languageCombo->addItem(tr("Polish"), "pl");

	restoreSettings();
}

void PreferencesDialog::updateHighlightColor()
{
	QRect iconRect = ui.highlightButton->contentsRect();
	QPixmap icon(iconRect.size() - QSize(20, 14));
	QPainter painter(&icon);
	painter.fillRect(icon.rect(), Qt::black);
	painter.fillRect(icon.rect().adjusted(1, 1, -1, -1), m_highlight);
	ui.highlightButton->setIconSize(icon.size());
	ui.highlightButton->setIcon(QIcon(icon));
}

void PreferencesDialog::updateFont()
{
	ui.fontLabel->setText(QString("%1 %2 ").arg(m_font.family()).arg(m_font.pointSize()));
}

void PreferencesDialog::selectHighlightColor()
{
	QColorDialog dlg(this);
	if (dlg.exec()) {
		m_highlight = dlg.currentColor();
		updateHighlightColor();
	}
}

void PreferencesDialog::selectFont()
{
	QFontDialog dlg(this);
	dlg.setCurrentFont(m_font);
	if (dlg.exec()) {
		m_font = dlg.selectedFont();
		updateFont();
	}
}

void PreferencesDialog::addServer()
{
	ui.serversList->addItem("");
	QListWidgetItem* item = ui.serversList->item(ui.serversList->count() - 1);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui.serversList->setCurrentItem(item);
	ui.serversList->editItem(item);
}

void PreferencesDialog::removeServer()
{
	if (ui.serversList->currentRow() != -1)
		delete ui.serversList->takeItem(ui.serversList->currentRow());
}

void PreferencesDialog::restoreSettings()
{
	QSettings settings;
	int lang = ui.languageCombo->findData(settings.value("Display/language").toString());
	ui.languageCombo->setCurrentIndex(qMax(lang, 0));
	ui.previewHeightSpin->setValue(settings.value("Display/previewHeight", 40).toInt());
	m_highlight = QColor(settings.value("Display/highlight", "#ffff00").toString());
	updateHighlightColor();

	m_font.fromString(settings.value("Display/textFont", m_font.toString()).toString());
	if (m_font.family().isEmpty())
		m_font = qApp->font();
	updateFont();

	ui.pathEdit->setText(settings.value("Tools/djviewPath", "djview").toString());
	ui.welcomeEdit->setText(settings.value("Help/welcome").toString());

	QStringList defaultServers;
	defaultServers << "poliqarp.wbl.klf.uw.edu.pl" << "poliqarp.kanji.klf.uw.edu.pl";
	ui.serversList->clear();
	ui.serversList->addItems(settings.value("Poliqarp/servers", defaultServers).toStringList());
}

void PreferencesDialog::saveSettings()
{
	QSettings settings;

	QString lang = ui.languageCombo->itemData(ui.languageCombo->currentIndex()).toString();
	if (lang != settings.value("Display/language").toString())
		MessageDialog::information(tr("The language will be changed after the application is restarted."));
	if (lang.isEmpty())
		settings.remove("Display/language");
	else settings.setValue("Display/language", lang);

	settings.setValue("Display/highlight", m_highlight.name());
	settings.setValue("Display/previewHeight", ui.previewHeightSpin->value());
	settings.setValue("Display/textFont", m_font.toString());
	settings.setValue("Tools/djviewPath", ui.pathEdit->text());
	settings.setValue("Help/welcome", ui.welcomeEdit->text());

	QStringList servers;
	for (int i = 0; i < ui.serversList->count(); i++)
		servers.append(ui.serversList->item(i)->text());
	settings.setValue("Poliqarp/servers", servers);


}
