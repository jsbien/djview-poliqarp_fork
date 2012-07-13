/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include "ui_helpdialog.h"

class HelpDialog : public QDialog
{
	Q_OBJECT

public:
	explicit HelpDialog(QWidget *parent = 0);
private:
	QString readFileContent(const QString& language) const;

	Ui::HelpDialog ui;
};

#endif // HELPDIALOG_H
