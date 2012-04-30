/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "preferencesdialog.h"
#include <QtGui>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
	 QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.highlightButton, SIGNAL(clicked()), this, SLOT(selectHighlightColor()));
	connect(ui.fontButton, SIGNAL(clicked()), this, SLOT(selectFont()));

	restoreSettings();
}

void PreferencesDialog::updateHighlightColor()
{
	QRect rect = ui.highlightButton->rect();
	QPixmap icon(rect.size() - QSize(10, 10));
	QPainter painter(&icon);
	painter.fillRect(0, 0, rect.width() - 8, rect.height() - 8, Qt::black);
	painter.fillRect(1, 1, rect.width() - 10, rect.height() - 10, m_highlight);
	ui.highlightButton->setIcon(QIcon(icon));
	ui.highlightButton->setIconSize(icon.size());
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
	QFont font = qApp->font();
	font.setFamily(ui.fontLabel->text());
	QFontDialog dlg(this);
	dlg.setCurrentFont(font);
	if (dlg.exec())
		ui.fontLabel->setText(dlg.selectedFont().family());
}

void PreferencesDialog::restoreSettings()
{
	QSettings settings;
	ui.pathEdit->setText(settings.value("Tools/djviewPath", "djview").toString());
	ui.previewHeightSpin->setValue(settings.value("Display/previewHeight", 40).toInt());
	m_highlight = QColor(settings.value("Display/highlight", "#ffff00").toString());
	ui.fontLabel->setText(settings.value("Display/font", qApp->font().family()).toString());
	updateHighlightColor();
}

void PreferencesDialog::saveSettings()
{
	QSettings settings;
	settings.setValue("Display/highlight", m_highlight.name());
	settings.setValue("Display/previewHeight", ui.previewHeightSpin->value());
	settings.setValue("Display/font", ui.fontLabel->text());
	settings.setValue("Tools/djviewPath", ui.pathEdit->text());

}
