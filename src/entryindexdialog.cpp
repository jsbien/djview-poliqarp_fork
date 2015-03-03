/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "entryindexdialog.h"

EntryIndexDialog::EntryIndexDialog(QWidget *parent) :
	QDialog(parent)
{
	ui.setupUi(this);
}

void EntryIndexDialog::setEntry(const Entry &entry)
{
	m_entry = entry;
	ui.entryEdit->setText(entry.word);
	ui.commentEdit->setText(entry.comment);
	ui.urlEdit->setText(entry.link.toString());
	ui.hiddenCheck->setChecked(!entry.isVisible());

	if (entry.word.isEmpty())
		ui.entryEdit->setFocus();
	else ui.commentEdit->setFocus();
}

Entry EntryIndexDialog::entry()
{
	m_entry.word = cleanString(ui.entryEdit->text());
	m_entry.comment = cleanString(ui.commentEdit->text());
	m_entry.link = QUrl(cleanString(ui.urlEdit->text()));
	if (ui.hiddenCheck->isChecked())
		m_entry.hide();
	else m_entry.show();
	return m_entry;
}

QString EntryIndexDialog::cleanString(const QString& s) const
{
	QString t = s;
	t.replace('\r', "");
	t.replace('\t', ' ');
	return t;
}
