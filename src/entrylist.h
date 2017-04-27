/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
