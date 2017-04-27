/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "entrylist.h"

EntryList::EntryList()
{
}

bool EntryList::open(const QString& filename)
{

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	clear();
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	while (!stream.atEnd()) {
		Entry entry = Entry::parse(stream.readLine());
		if (entry.isValid())
			append(entry);
	}
	return true;
}

bool EntryList::save(const QString& filename)
{
	const QString backup = filename + ".bak";
	if (QFile::exists(filename) && !m_backedUp.contains(filename)) {
		m_backedUp.insert(filename);
		if (QFile::exists(backup))
			QFile::remove(backup);
		QFile::rename(filename, backup);
	}

	QFile file(filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		stream.setCodec("UTF-8");
		QList<Entry> entries;
		for (int i = 0; i < count(); i++)
			if (!at(i).isDeleted()) {
				stream << at(i).toString();
				//entries.append(m_entries[i]);
			}
		//std::swap(m_entries, entries);
		//sort();
		//m_modified = false;
		return true;
	}
	else {
		if (QFile::exists(backup))
			QFile::rename(backup, filename);
		return false;
	}
}

QSet<QString> EntryList::m_backedUp;
