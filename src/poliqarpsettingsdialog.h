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
	void restoreSettings(const QUrl &corpusUrl);
	void saveSettings();
private:
	/** @return group header. */
	QString group() const;
	Ui::PoliqarpSettingsDialog ui;
	QString m_corpusUrl;
};

#endif // POLIQARPSETTINGSDIALOG_H
