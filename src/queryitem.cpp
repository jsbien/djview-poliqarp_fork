/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "queryitem.h"

QueryItem::QueryItem()
{
}

void QueryItem::setLink(const QUrl &link)
{
	m_link = link;
//	m_link.setEncodedQueryItems(QList<QPair<QByteArray, QByteArray> >());

	QPair<QString, QString> arg;
	foreach (arg, link.queryItems()) {
		if (arg.first == "page")
			m_page = arg.second.toInt();
		else if (arg.first == "highlight") {
			m_highlighted.setLeft(arg.second.section(',', 0, 0).toInt());
			m_highlighted.setTop(arg.second.section(',', 1, 1).toInt());
			m_highlighted.setWidth(arg.second.section(',', 2, 2).toInt());
			m_highlighted.setHeight(arg.second.section(',', 3, 3).toInt());
		}
		else if (arg.first == "showposition") {
			m_position.setX(arg.second.section(QRegExp("[.,]"), 1, 1).toInt());
			m_position.setY(arg.second.section(QRegExp("[.,]"), 3, 3).toInt());
		}
	}
}
