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

#pragma once

#include "ui_entryindexdialog.h"
#include "entry.h"

class EntryIndexDialog : public QDialog
{
	Q_OBJECT
public:
	explicit EntryIndexDialog(QWidget *parent = nullptr);
	void setEntry(const Entry& entry);
	Entry entry();
private:
	QString cleanString(const QString& s) const;
	Entry m_entry;
	Ui::EntryIndexDialog ui;
};

