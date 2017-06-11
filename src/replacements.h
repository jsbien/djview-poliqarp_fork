/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#pragma once
#include <QString>
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
};
