/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QColor>
#include <QGuiApplication>
#include "entry.h"
#include "indexmodel.h"

IndexModel::IndexModel(QObject* parent) : QAbstractListModel(parent)
{
	configure();
	m_sortingMethod = SortByIndex;
}

int IndexModel::rowCount(const QModelIndex& index) const
{
	return index.isValid() ? 0 : m_data.count();
}

QVariant IndexModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	if (row < 0 || row >= m_data.count())
		return QVariant();

	switch (role) {
	case Qt::DisplayRole:
	case Qt::EditRole:
		return m_sortingMethod == SortAtergo ? m_data[row].word() : m_data[row].title();
	case EntryCommentRole:
		return m_data[row].comment();
	case Qt::ForegroundRole:
		return m_data[row].validLink().isValid() ? QColor(Qt::black) : QColor(Qt::gray);
	case Qt::FontRole:
		return m_fonts.value(m_data[row].isDeleted());
	case Qt::TextAlignmentRole:
		return int((m_sortingMethod == SortAtergo ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter);
	case EntryLinkRole:
		return m_data[row].validLink();
	case EntryDeletedRole:
		return m_data[row].isDeleted();
	case EntrySortRole:
		return sortingKey(m_data[row]);
	default:
		return QVariant();
	}


//	if (ui.actionAtergoOrder->isChecked())
//		item->setTextAlignment(Qt::AlignRight);

//	bool hasComment = entry.comment.isEmpty();
//	item->setIcon(hasComment ? m_commentIcon : QIcon());

}

bool IndexModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	int row = index.row();
	Entry entry = m_data[row];
	switch (role) {
	case Qt::EditRole:
		entry.setWord(value.toString());
		break;
	case EntryLinkRole:
		entry.setLink(value.toUrl());
		break;
	case EntryCommentRole:
		entry.setComment(value.toString());
		break;
	case EntryDeletedRole:
		entry.setDeleted(value.toBool());
		break;
	default:
		return false;
	}
	setEntry(index, entry);
	return true;
}

bool IndexModel::open(const QString& filename)
{
	beginResetModel();
	bool success = m_data.open(filename);
	endResetModel();
	if (success)
		m_undo.clear();
	return success;
}

bool IndexModel::save(const QString& filename)
{
	if (!m_data.save(filename))
		return false;
	m_undo.clear();
	return true;
}

void IndexModel::clear()
{
	beginResetModel();
	m_data.clear();
	m_undo.clear();
	endResetModel();
}

Entry IndexModel::entry(const QModelIndex& index) const
{
	return m_data.value(index.row());
}

void IndexModel::setEntry(const QModelIndex& index, const Entry& entry)
{
	if (m_data[index.row()].isCopyOf(entry))
		return;
	if (!m_undo.contains(index.row()))
		m_undo[index.row()] = m_data[index.row()];
	else if (entry.isCopyOf(m_undo[index.row()]))
		m_undo.remove(index.row());
	m_data[index.row()] = entry;
	emit dataChanged(index, index);
}

bool IndexModel::isModified(const QModelIndex& index) const
{
	return m_undo.contains(index.row());
}

void IndexModel::restoreEntry(const QModelIndex& index)
{
	if (isModified(index)) {
		m_data[index.row()] = m_undo[index.row()];
		m_undo.remove(index.row());
		emit dataChanged(index, index);
	}
}

void IndexModel::addEntry(const Entry& entry)
{
	int row = m_data.count();
	beginInsertRows(QModelIndex(), row, row);
	m_data.append(entry);
	endInsertRows();
}

void IndexModel::addEntries(const EntryList& entries)
{
	beginResetModel();
	m_data.append(entries);
	endResetModel();
}

void IndexModel::setSortingMethod(IndexModel::SortMethod method)
{
	m_sortingMethod = method;
}

void IndexModel::configure()
{
	m_fonts.clear();
	QFont font;
	font.setStyleStrategy(QFont::NoFontMerging);
	font.fromString(QSettings().value("Display/textFont", font.toString()).toString());
	if (font.family().isEmpty())
		font = qApp->font();
	m_fonts.append(font);
	font.setStrikeOut(true);
	m_fonts.append(font);
}


QString IndexModel::sortingKey(const Entry& e) const
{
	switch (m_sortingMethod) {
	case SortByIndex:
		return QString();
	case SortAlphabetically:
		return e.title();
	case SortAtergo:
		return aTergo(e.word());
	case SortByLetters:
		return QString(e.title()).remove(' ');
	}
	return QString();
}

QString IndexModel::aTergo(const QString& s) const
{
	QString t = s;
	std::reverse(t.begin(), t.end());
	return t;
}
