/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARPSETTINGSDIALOG_H
#define POLIQARPSETTINGSDIALOG_H

#include "ui_poliqarpsettingsdialog.h"

class PoliqarpSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PoliqarpSettingsDialog(QWidget *parent = 0);
	void restoreSettings(const QString& server);
	void saveSettings();
private:
	Ui::PoliqarpSettingsDialog ui;
	QString m_serverUrl;
};

#endif // POLIQARPSETTINGSDIALOG_H
