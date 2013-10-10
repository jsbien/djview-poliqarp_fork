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

QList<FileIndex::Entry> FileIndex::items(int flags) const
{
	QList<Entry> results;
	for (int i = 0; i < m_entries.count(); i++)
		if (flags & ViewHidden || m_entries[i].isVisible())
			results.append(m_entries[i]);

	// Sorting
	if (flags & AlphabeticOrder)
		qSort(results.begin(), results.end(), AlphabeticComparator());
	else if (flags & AtergoOrder)
		qSort(results.begin(), results.end(), AtergoComparator());

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

bool FileIndex::AtergoComparator::operator()(const FileIndex::Entry &e1, const FileIndex::Entry &e2)
{
	int w1, w2;
	for (w1 = e1.word.count() - 1, w2 = e2.word.count() - 1;
		  w1 >= 0 && w2 >= 0; w1--, w2--)
		if (e1.word[w1] != e2.word[w2])
			return e1.word[w1] < e2.word[w2];
	if (w1 < 0)
		return w2 >= 0;
	else return false;

}
