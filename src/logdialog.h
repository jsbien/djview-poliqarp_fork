/****************************************************************************
*   Copyright (C) 2012-2018 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#include "ui_logdialog.h"

class LogDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LogDialog(QWidget *parent = nullptr);
	void setText(const QString &text);
	bool isCleared() const {return ui.logBrowser->toPlainText().trimmed().isEmpty();}
protected:
	void hideEvent(QHideEvent* event);
private slots:
	void copy();
private:
	Ui::LogDialog ui;
};

