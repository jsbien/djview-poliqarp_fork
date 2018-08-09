/****************************************************************************
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
