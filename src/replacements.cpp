/****************************************************************************
*   Copyright (C) 2018 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
