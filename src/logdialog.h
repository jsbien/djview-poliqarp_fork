/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include "ui_logdialog.h"

class LogDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LogDialog(QWidget *parent = 0);
	void setText(const QString &text);

private:
	Ui::LogDialog ui;
};

#endif // LOGDIALOG_H
