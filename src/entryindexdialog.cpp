/****************************************************************************
*   Copyright (C) 2013-2018 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
	ui.entryEdit->setText(entry.word());
   ui.commentEdit->setPlainText(entry.comment());
	ui.urlEdit->setText(entry.link().toString());
   ui.descriptionEdit->setPlainText(entry.description());

	if (entry.word().isEmpty())
		ui.entryEdit->setFocus();
	else ui.commentEdit->setFocus();
}

Entry EntryIndexDialog::entry()
{
	m_entry.setWord(cleanString(ui.entryEdit->text()));
   m_entry.setComment(cleanString(ui.commentEdit->toPlainText()));
	m_entry.setLink(QUrl(cleanString(ui.urlEdit->text())));
   m_entry.setDescription(cleanString(ui.descriptionEdit->toPlainText()));
	return m_entry;
}

QString EntryIndexDialog::cleanString(const QString& s) const
{
	QString t = s;
	t.replace(QRegExp("\\s+"), " ");
	return t.trimmed();
}
