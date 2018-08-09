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
#include <QString>
#include <QUrl>
#include <QList>

struct Replacement {
	QString from;
	QString to;
	Replacement() {}
	Replacement(const QString& line);
	bool isValid() const;
	QString expand(const QString& previous, int cursor, const QString& suffix) const;
};


class Replacements : public QList<Replacement>
{
public:
	Replacements();
	Replacements(const QString& value);
	void read(const QString& value);
	QUrl replace(const QUrl& url) const;
};
