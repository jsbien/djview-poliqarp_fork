/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "dictionaryindex.h"

DictionaryIndex::DictionaryIndex()
{
	m_modified = false;
}

bool DictionaryIndex::open(const QString &dictionaryFile)
{
	clear();

	QFile file(dictionaryFile);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	while (!stream.atEnd())
		addEntry(stream.readLine());
	if (m_entries.isEmpty())
		return false;
	m_filename = dictionaryFile;
	m_modified = false;
	return true;
}

bool DictionaryIndex::save()
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

void DictionaryIndex::clear()
{
	m_entries.clear();
	m_filename.clear();
	m_modified = false;
}

QStringList DictionaryIndex::find(const QString &word) const
{
	QStringList results;
	for (int i = 0; i < m_entries.count(); i++)
		if (m_entries[i].word.startsWith(word) && m_entries[i].isVisible())
			results.append(m_entries[i].formattedWord());
	return results;
}

QUrl DictionaryIndex::url(const QString &word) const
{
	for (int i = 0; i < m_entries.count(); i++)
		if (m_entries[i].word == word)
			return m_entries[i].link;
	return QUrl();
}

void DictionaryIndex::hide(const QString &entry)
{
	int index = m_entries.indexOf(entry);
	if (index != -1) {
		m_entries[index].hide();
		m_modified = true;
	}
}

bool DictionaryIndex::setLink(const QString &word, const QUrl &link)
{
	int index = m_entries.indexOf(word);
	if (index == -1)
		return false;
	m_entries[index].link = link;
	m_modified = true;
	return true;
}

bool DictionaryIndex::addWord(const QString& word, const QUrl &link)
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

void DictionaryIndex::addEntry(const QString &line)
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


QString DictionaryIndex::Entry::toString()
{
	if (link.isEmpty())
		return QString("%1;-;%2\n").arg(word).arg(comment);
	else return QString("%1;%2;%3\n").arg(word).arg(link.toString()).arg(comment);
}
