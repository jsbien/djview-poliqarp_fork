/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "entryindexdialog.h"

EntryIndexDialog::EntryIndexDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}

void EntryIndexDialog::setEntry(const FileIndex::Entry &entry)
{
	m_entry = entry;
	ui.commentEdit->setText(entry.comment);
	ui.urlEdit->setText(entry.link.toString());
	ui.hiddenCheck->setChecked(!entry.isVisible());
}

FileIndex::Entry EntryIndexDialog::entry()
{
	m_entry.comment = ui.commentEdit->text();
	m_entry.link = QUrl(ui.urlEdit->text());
	if (ui.hiddenCheck->isChecked())
		m_entry.hide();
	else m_entry.show();
	return m_entry;
}
