/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "queryresultmodel.h"
#include "poliqarp.h"

QueryResultModel::QueryResultModel(Poliqarp* poliqarp, QObject *parent) :
	QAbstractListModel(parent), m_poliqarp(poliqarp)
{
}

int QueryResultModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else return m_poliqarp->queryCount();
}

QVariant QueryResultModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || role != Qt::DisplayRole)
		return QVariant();
	return m_poliqarp->query(index.row()).text();
}
