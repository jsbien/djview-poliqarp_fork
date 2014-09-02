/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef ENTRYINDEXDIALOG_H
#define ENTRYINDEXDIALOG_H

#include "ui_entryindexdialog.h"
#include "entry.h"

class EntryIndexDialog : public QDialog
{
	Q_OBJECT	
public:
	explicit EntryIndexDialog(QWidget *parent = 0);
	void setEntry(const Entry& entry);
	Entry entry();
private:
	Entry m_entry;
	Ui::EntryIndexDialog ui;
};

#endif // ENTRYINDEXDIALOG_H
