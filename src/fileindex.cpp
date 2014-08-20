/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "fileindex.h"

FileIndex::FileIndex()
{
	m_modified = false;
}

bool FileIndex::open(const QString &filename)
{
	clear();

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	while (!stream.atEnd()) {
		Entry entry = parseEntry(stream.readLine());
		if (entry.isValid())
			m_entries.append(entry);
	}
	if (m_entries.isEmpty())
		return false;
	m_filename = filename;
	m_modified = false;
	return true;
}

bool FileIndex::save()
{
	const QString backup = m_filename + ".bak";
	if (QFile::exists(m_filename) && !m_backedUp.contains(m_filename)) {
		m_backedUp.insert(m_filename);
		QFile::rename(m_filename, backup);
	}

	QFile file(m_filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		stream.setCodec("UTF-8");
		for (int i = 0; i < m_entries.count(); i++)
			stream << m_entries[i].toString();
		m_modified = false;
		return true;
	}
	else {
		if (QFile::exists(backup))
			QFile::rename(backup, m_filename);
		return false;
	}
}

void FileIndex::clear()
{
	m_entries.clear();
	m_filename.clear();
	m_modified = false;
}

QList<FileIndex::Entry> FileIndex::items(int flags) const
{
	QList<Entry> results;
	for (int i = 0; i < m_entries.count(); i++)
		if (flags & ViewHidden || m_entries[i].isVisible())
			results.append(m_entries[i]);

	// Reverse for a tergo
	bool reverse = flags & AtergoOrder;
	if (reverse) {
		for (int i = 0; i < results.count(); i++)
			results[i].word = reverseString(results[i].word);
	}

	// Sorting
	//	QTime timer;
	//	timer.start();
	if (flags & (AlphabeticOrder | AtergoOrder))
		qSort(results.begin(), results.end(), AlphabeticComparator());
	//	qDebug("Sorting %d items in %.2f s.", results.count(), timer.elapsed() / 1000.0);

	// Unreverse for a tergo
	if (reverse) {
		for (int i = 0; i < results.count(); i++)
			results[i].word = reverseString(results[i].word);
	}

	return results;
}

void FileIndex::hide(const QString &entry)
{
	int index = m_entries.indexOf(entry);
	if (index != -1) {
		m_entries[index].hide();
		m_modified = true;
	}
}

void FileIndex::show(const QString &entry)
{
	int index = m_entries.indexOf(entry);
	if (index != -1) {
		m_entries[index].show();
		m_modified = true;
	}
}

QUrl FileIndex::link(const QString &word) const
{
	int index = m_entries.indexOf(word);
	return index == -1 ? QUrl() : m_entries[index].link;
}

bool FileIndex::setLink(const QString &word, const QUrl &link)
{
	int index = m_entries.indexOf(word);
	if (index == -1)
		return false;
	m_entries[index].link = link;
	m_modified = true;
	return true;
}

QString FileIndex::comment(const QString& word) const
{
	int index = m_entries.indexOf(word);
	if (index == -1)
		return QString();
	else if (m_entries[index].isVisible())
		return m_entries[index].comment;
	else return m_entries[index].comment.mid(1);
}

bool FileIndex::setComment(const QString &word, const QString &comment)
{
	int index = m_entries.indexOf(word);
	if (index == -1)
		return false;
	m_entries[index].comment = m_entries[index].isVisible() ?
											comment : QString("!%1").arg(comment);
	m_modified = true;
	return true;
}

bool FileIndex::addEntry(const Entry& entry)
{
	int index = m_entries.indexOf(entry.word);
	if (index != -1)
		return false;
	m_entries.append(entry);
	m_modified = true;
	return true;
}

FileIndex::Entry FileIndex::entry(const QString &word) const
{
	int index = m_entries.indexOf(word);
	return index != -1 ? m_entries[index] : Entry();
}

void FileIndex::setEntry(const QString& word, const FileIndex::Entry& entry)
{
	int index = m_entries.indexOf(word);
	if (index != -1)
		m_entries[index] = entry;
	m_modified = true;
}

FileIndex::Entry FileIndex::parseEntry(const QString &line) const
{
	QStringList parts = csvToStringList(line);
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

QString FileIndex::reverseString(const QString& s) const
{
	QString reversed;
	reversed.reserve(s.count());
	for (int c = s.count() - 1; c >= 0; c--)
		reversed.append(s[c]);
	return reversed;
}

QStringList FileIndex::csvToStringList(const QString& row)
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

QString FileIndex::stringListToCsv(const QStringList& columns)
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

QString FileIndex::Entry::toString()
{
	QStringList columns;
	columns.append(word);
	columns.append(link.isEmpty() ? QString("-") : link.toString());
	columns.append(comment);
	return FileIndex::stringListToCsv(columns);
}

QSet<QString> FileIndex::m_backedUp;
