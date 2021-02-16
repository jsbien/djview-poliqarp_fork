/****************************************************************************
 *   This software is subject to, and may be distributed under, the
 *   GNU General Public License, either version 2 of the license,
 *   or (at your option) any later version. The license should have
 *   accompanied the software or you may obtain a copy of the license
 *   from the Free Software Foundation at http://www.fsf.org .
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
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

Entry::Entry() : m_deleted(false)
{
}

Entry::Entry(const QString& w) : m_word(w.trimmed()), m_deleted(false)
{
}

QString Entry::word() const
{
	return m_word;
}

void Entry::setWord(const QString& word)
{
	m_word = word;
}

QUrl Entry::link() const
{
	return m_link;
}

void Entry::setLink(const QUrl& link)
{
	m_link = link;
}

QString Entry::comment() const
{
	return m_comment;
}

void Entry::setComment(const QString& comment)
{
	m_comment = comment;
}

QString Entry::description() const
{
	return m_description;
}

void Entry::setDescription(const QString& description)
{
	m_description = description;
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
	columns.append(m_word);
	columns.append(m_link.isEmpty() ? QString("-") : m_link.toString());
	columns.append(m_comment);
	columns.append(m_description);
	return stringListToCsv(columns);
}

QString Entry::title() const
{
	return m_description.isEmpty() ? m_word : m_word + ' ' + m_description;
}

bool Entry::isCopyOf(const Entry& e) const
{
	return m_word == e.m_word && m_comment == e.m_comment && m_description == e.m_description &&
			 m_link == e.m_link && m_deleted == e.m_deleted;
}

QUrl Entry::validLink() const
{
	if (m_link.scheme() == "file" || m_link.scheme() == "http" || m_link.scheme() == "https" ||
		 m_link.scheme() == "ftp")
		return m_link;
	else
		return QUrl();
}

Entry Entry::parse(const QString& line)
{
	QStringList parts = Entry::csvToStringList(line);
	if (parts.count() < 2)
		return Entry();
	Entry entry;
	entry.m_word = parts[0].trimmed();
	if (!parts[1].startsWith('-'))
		entry.m_link = parts[1];
	if (parts.count() > 2)
		entry.m_comment = parts[2];
	if (parts.count() > 3)
		entry.m_description = parts[3];
	return entry;
}
