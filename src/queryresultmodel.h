/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef QUERYRESULTMODEL_H
#define QUERYRESULTMODEL_H

#include <QtCore>

class Poliqarp;

class QueryResultModel : public QAbstractListModel
{
	 Q_OBJECT
public:
	explicit QueryResultModel(Poliqarp* poliqarp, QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
signals:

public slots:

private:
	Poliqarp* m_poliqarp;
};

#endif // QUERYRESULTMODEL_H
