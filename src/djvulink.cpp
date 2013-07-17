/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "djvulink.h"

DjVuLink::DjVuLink()
{
}

void DjVuLink::setLink(const QUrl &link)
{
	m_link = link;
	QPair<QString, QString> arg;
	foreach (arg, link.queryItems()) {
		if (arg.first == "page")
			m_page = qMax(0, arg.second.toInt() - 1);
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

QString DjVuLink::documentPath() const
{
	return m_link.scheme() + "://" + m_link.host() + m_link.path();
}

QString DjVuLink::toCsv(const QChar &separator) const
{
	if (!isValid())
		return QString();
	QStringList fields;
	fields << m_leftContext << m_match << m_rightMatch << m_rightContext << m_link.toString();
	for (int i = 0; i < fields.count(); i++)
		if (fields[i].contains(separator)) {
			fields[i].replace("\"", "\"\"");
			fields[i] = QString("\"\%1\"").arg(fields[i]);
		}
	return fields.join(separator) + "\n";
}

