/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "ui_preferencesdialog.h"

class PreferencesDialog : public QDialog
{
	 Q_OBJECT
public:
	explicit PreferencesDialog(QWidget *parent = 0);
	void restoreSettings();
	void saveSettings();
private slots:
	void selectHighlightColor();
	void selectFont();
	void addServer();
	void removeServer();
private:
	void updateHighlightColor();
	void updateFont();
	Ui::PreferencesDialog ui;
	QColor m_highlight;
	QFont m_font;
};

#endif // PREFERENCESDIALOG_H
