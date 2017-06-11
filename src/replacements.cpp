/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
