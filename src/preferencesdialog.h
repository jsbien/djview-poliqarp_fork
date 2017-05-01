/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#pragma once
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

