/****************************************************************************
*   Copyright (C) 2014 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "entry.h"

QStringList Entry::csvToStringList(const QString& row)
{
	QStringList items;
	int start = 0;
	bool quoted = false;
	for (int i = 0; i < row.count(); i++) {
		if (!quoted && row[i] == ';') {
			QString item = row.mid(start, i - start);
			if (item.endsWith('"'))
				item.truncate(item.count() - 1);
			item.replace("\"\"", "\"");
			items.append(item);
			start = i + 1;
		}
		else if (row[i] == '"') {
			if (!quoted && start == i)
				start++;
			quoted = !quoted;
		}
	}
	if (start < row.count())
		items.append(row.mid(start));
	return items;
}

QString Entry::stringListToCsv(const QStringList& columns)
{
	QStringList formatted = columns;
	for (int i = 0; i < formatted.count(); i++)
		if (formatted[i].contains(';')) {
			QString quoted = formatted[i];
			quoted.replace("\"", "\"\"");
			formatted[i] = QString("\"%1\"").arg(quoted);
		}
	return formatted.join(";") + QChar('\n');
}

void Entry::setDeleted(bool deleted)
{
	m_deleted = deleted;
}

bool Entry::isDeleted() const
{
	return m_deleted;
}

QString Entry::toString() const
{
	QStringList columns;
	columns.append(word);
	columns.append(link.isEmpty() ? QString("-") : link.toString());
	columns.append(comment);
	return stringListToCsv(columns);
}

QUrl Entry::validLink() const
{
	if (link.scheme() == "file" || link.scheme() == "http" ||
		 link.scheme() == "https" || link.scheme() == "ftp")
		return link;
	else return QUrl();
}

Entry Entry::parse(const QString& line)
{
	QStringList parts = Entry::csvToStringList(line);
	if (parts.count() < 2)
		return Entry();
	Entry entry;
	entry.word = parts[0].trimmed();
	if (!parts[1].startsWith('-'))
		entry.link = parts[1];
	if (parts.count() > 2)
		entry.comment = parts[2];
	return entry;

}



