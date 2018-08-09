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
#include "replacements.h"


Replacement::Replacement(const QString& line)
{
	from = line.section('=', 0, 0);
	to = line.section('=', 1).trimmed();
}

bool Replacement::isValid() const
{
	return !from.isEmpty() && !to.isEmpty();
}

QString Replacement::expand(const QString& previous, int cursor, const QString& suffix) const
{
	return previous.left(cursor - (from.length() - suffix.length())) + to + previous.mid(cursor);
}


Replacements::Replacements()
{
}

Replacements::Replacements(const QString& value)
{
	read(value);
}

void Replacements::read(const QString& value)
{
	clear();
	for (const QString& line: value.split('\n', QString::SkipEmptyParts)) {
		Replacement replacement(line);
		if (replacement.isValid())
			append(replacement);
	}
}

QUrl Replacements::replace(const QUrl& url) const
{
	QString path = url.toString();
	foreach (const Replacement& r, *this)
		if (path.startsWith(r.from)) {
			path = r.to + path.mid(r.from.count());
			break;
		}
	return QUrl(path);
}
