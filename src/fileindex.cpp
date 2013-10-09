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
	while (!stream.atEnd())
		addEntry(stream.readLine());
	if (m_entries.isEmpty())
		return false;
	m_filename = filename;
	m_modified = false;
	return true;
}

bool FileIndex::save()
{
	bool backup = false;
	if (QFile::exists(m_filename)) {
		QFile::rename(m_filename, m_filename + "~");
		backup = true;
	}

	QFile file(m_filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	for (int i = 0; i < m_entries.count(); i++)
		stream << m_entries[i].toString();

	if (backup)
		QFile::remove(m_filename + "~");

	m_modified = false;
	return true;
}

void FileIndex::clear()
{
	m_entries.clear();
	m_filename.clear();
	m_modified = false;
}

QStringList FileIndex::items() const
{
	QStringList results;
	for (int i = 0; i < m_entries.count(); i++)
		if (m_entries[i].isVisible())
			results.append(m_entries[i].formattedWord());
	return results;
}

QUrl FileIndex::url(const QString &word) const
{
	for (int i = 0; i < m_entries.count(); i++)
		if (m_entries[i].word == word)
			return m_entries[i].link;
	return QUrl();
}

void FileIndex::hide(const QString &entry)
{
	int index = m_entries.indexOf(entry);
	if (index != -1) {
		m_entries[index].hide();
		m_modified = true;
	}
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

bool FileIndex::addWord(const QString& word, const QUrl &link)
{
	int index = m_entries.indexOf(word);
	if (index != -1)
		return false;
	Entry entry(word);
	entry.link = link;
	m_entries.append(entry);
	m_modified = true;
	return true;
}

void FileIndex::addEntry(const QString &line)
{
	QStringList parts = line.split(";");
	if (parts.count() < 2)
		return;
	Entry entry;
	entry.word = parts[0].trimmed();
	if (!parts[1].startsWith('-'))
		entry.link = parts[1];
	if (parts.count() > 2)
		entry.comment = parts[2];
	m_entries.append(entry);
	m_modified = true;
}


QString FileIndex::Entry::toString()
{
	if (link.isEmpty())
		return QString("%1;-;%2\n").arg(word).arg(comment);
	else return QString("%1;%2;%3\n").arg(word).arg(link.toString()).arg(comment);
}
