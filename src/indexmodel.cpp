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
	switch (role) {
	case Qt::EditRole:
		m_data[row].setWord(value.toString());
		break;
	case EntryLinkRole:
		m_data[row].setLink(value.toUrl());
		break;
	case EntryCommentRole:
		m_data[row].setComment(value.toString());
		break;
	case EntryDeletedRole:
		m_data[row].setDeleted(value.toBool());
		break;
	default:
		return false;
	}
	return true;
}

bool IndexModel::open(const QString& filename)
{
	beginResetModel();
	bool success = m_data.open(filename);
	endResetModel();
	return success;
}

bool IndexModel::save(const QString& filename)
{
	return m_data.save(filename);
}

void IndexModel::clear()
{
	beginResetModel();
	m_data.clear();
	endResetModel();
}

Entry IndexModel::entry(const QModelIndex& index) const
{
	return m_data.value(index.row());
}

void IndexModel::setEntry(const QModelIndex& index, const Entry& entry)
{
	m_data[index.row()] = entry;
	emit dataChanged(index, index);
}

void IndexModel::addEntry(const Entry& entry)
{
	int row = m_data.count();
	beginInsertRows(QModelIndex(), row, row);
	m_data.append(entry);
	endInsertRows();
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
