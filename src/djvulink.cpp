/****************************************************************************
*   Copyright (C) 2012-2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#include "djvulink.h"

DjVuLink::DjVuLink()
{
}

void DjVuLink::setLink(const QUrl &link)
{
	// Link example:
	// http://ebuw.uw.edu.pl/Content/234/directory.djvu?djvuopts&page=218&zoom=width&showposition=0.354,0.750&highlight=964,990,97,35#SW
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

QUrl DjVuLink::colorRegionLink(const QRect& rect) const
{
	QString url = m_link.toString();
	QString highlight = QString("highlight=%1,%2,%3,%4,%5").arg(rect.left())
								  .arg(rect.top()).arg(rect.width()).arg(rect.height())
							  .arg(QSettings().value("Display/highlight", "#ffff00").toString().mid(1));
	QRegExp reg("highlight=\\d+,\\d+,\\d+,\\d+");
	url.replace(reg, highlight);
	qDebug() << url;
	return QUrl(url);
}

QUrl DjVuLink::regionLink(const QRect& rect) const
{
	QString url = m_link.toString();
	QString highlight = QString("highlight=%1,%2,%3,%4").arg(rect.left())
								  .arg(rect.top()).arg(rect.width()).arg(rect.height());
	QRegExp reg("highlight=\\d+,\\d+,\\d+,\\d+");
	url.replace(reg, highlight);
	return QUrl(url);
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

