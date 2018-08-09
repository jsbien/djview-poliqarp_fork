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
#include "entry.h"

class EntryList : public QList<Entry>
{
public:
	EntryList();
	bool open(const QString& filename);
	bool save(const QString& filename);
private:
	QString m_filename;
	static QSet<QString> m_backedUp;
};
