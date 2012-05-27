/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "poliqarpsettingsdialog.h"
#include <QtCore>

PoliqarpSettingsDialog::PoliqarpSettingsDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}

void PoliqarpSettingsDialog::restoreSettings(const QString &server)
{
	m_serverUrl = server;

	QSettings settings;
	settings.beginGroup(server);

	ui.randomSampleCheck->setChecked(settings.value("random_sample", false).toBool());
	ui.randomSampleSpin->setValue(settings.value("random_sample_size", 50).toInt());

	ui.sortGroup->setChecked(settings.value("sort", false).toBool());
	QString column = settings.value("sort_column", "lc").toString();
	if (column == "lc")
		ui.leftContextSortRadio->setChecked(true);
	else if (column == "lm")
		ui.leftMatchSortRadio->setChecked(true);
	else if (column == "rc")
		ui.rightContextSortRadio->setChecked(true);
	else ui.rightMatchSortRadio->setChecked(true); // rm
	QString type = settings.value("sort_type", "afronte").toString();
	if (type == "afronte")
		ui.afronteSortRadio->setChecked(true);
	else ui.atergoSortRadio->setChecked(true);
	QString direction = settings.value("sort_direction", "asc").toString();
	if (direction == "asc")
		ui.ascendingSortRadio->setChecked(true);
	else ui.descendingSortRadio->setChecked(true);

	QString match = settings.value("show_in_match", "s").toString();
	ui.primaryMatchCheck->setChecked(match.contains('s'));
	ui.derivedMatchCheck->setChecked(match.contains('l'));
	ui.tagMatchCheck->setChecked(match.contains('t'));

	QString context =	settings.value("show_in_context", "s").toString();
	ui.primaryContextCheck->setChecked(context.contains('s'));
	ui.derivedContextCheck->setChecked(context.contains('l'));
	ui.tagContextCheck->setChecked(context.contains('t'));

	ui.leftWidthSpin->setValue(settings.value("left_context_width", 5).toInt());
	ui.rightWidthSpin->setValue(settings.value("right_context_width", 5).toInt());
	ui.contextWidthSpin->setValue(settings.value("wide_context_width", 50).toInt());
}

void PoliqarpSettingsDialog::saveSettings()
{
	QSettings settings;
	settings.beginGroup(m_serverUrl);

	settings.setValue("random_sample", ui.randomSampleCheck->isChecked());
	settings.value("random_sample_size", ui.randomSampleSpin->value());

	settings.value("sort", ui.sortGroup->isChecked());
	QString column;
	if (ui.leftMatchSortRadio->isChecked())
		column = "lm";
	else if (ui.leftContextSortRadio->isChecked())
		column = "lc";
	else if (ui.rightMatchSortRadio->isChecked())
		column = "rm";
	else column = "rc";
	settings.setValue("sort_column", column);
	settings.setValue("sort_type", ui.afronteSortRadio->isChecked() ?
								"afronte" : "atergo");
	settings.setValue("sort_direction", ui.ascendingSortRadio->isChecked() ?
								"asc" : "desc");

	QString match;
	if (ui.primaryMatchCheck->isChecked())
		match.append('s');
	if (ui.derivedMatchCheck->isChecked())
		match.append('l');
	if (ui.tagMatchCheck->isChecked())
		match.append('t');
	settings.setValue("show_in_match", match);
	QString context;
	if (ui.primaryContextCheck->isChecked())
		context.append('s');
	if (ui.derivedContextCheck->isChecked())
		context.append('l');
	if (ui.tagContextCheck->isChecked())
		context.append('t');
	settings.setValue("show_in_context", context);

	settings.setValue("left_context_width", ui.leftWidthSpin->value());
	settings.setValue("right_context_width", ui.rightWidthSpin->value());
	settings.setValue("wide_context_width", ui.contextWidthSpin->value());

	// Fixed
	settings.setValue("graphical_concordances", 0);
	settings.setValue("results_per_page", 25);
}
