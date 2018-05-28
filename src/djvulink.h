/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

#pragma once
#include <QtCore>
#include <QColor>
#include <libdjvu/ddjvuapi.h>

/** The DjVuLink class contains a link to external DjVu document
	 with optional information like context and highlighting. */
class DjVuLink
{
public:
	struct Highlight {
		int page;
		QRect rect;
		QColor color;
	};

	DjVuLink();
	DjVuLink(QUrl link)	{setLink(link);}
	bool isValid() const {return m_link.isValid();}
	int columns() const {return 4 - m_rightMatch.isEmpty();}
	void setLeftContext(const QString& context)	{m_leftContext = context.trimmed();}
	QString leftContext()	{return m_leftContext;}
	void setRightContext(const QString& context)	{m_rightContext = context.trimmed();}
	QString rightContext()	{return m_rightContext;}
	void setMatch(const QString& word)	{m_match = word.trimmed();}
	QString match() const	{return m_match;}
	void setRightMatch(const QString& word)	{m_rightMatch = word.trimmed();}
	QString rightMatch() const	{return m_rightMatch;}

	/** Return highlights. Note: highlights aren't bound to page until setPage is called */
	QList<Highlight> highlights() const;

	void setLink(const QUrl& link);
	/** @return a link to original part of DjVu page. */
	QUrl link() const;
	/** @return a link to main DjVu document. */
	QUrl downloadLink() const;
	/** @return a link to given part of DjVu page with selected highlight using configured highlight color. */
	QUrl colorRegionLink(const QRect &rect, int page = -1) const;
	/** @return a link to given part of DjVu page with selected highlight. */
	QUrl regionLink(const QRect &rect, int page = -1) const;
	QString documentPath() const;
	int page() const {return m_page;}
	/** @return identificator of page used in link */
	QString pageId() const { return m_pageId; }
	void setPage(int page);
	/** Set page by name */
	void setPage(const QString& pageName, const QList<ddjvu_fileinfo_t>& documentPages);

	QPoint position() const {return m_position;}
	QString text() const	{return m_leftContext + ' ' + m_match + ' ' + m_rightContext;}
	void setMetadata(const QString& text) {m_metadata = text.trimmed();}
	QString metadata() const {return m_metadata;}

	/** Export results to CSV. EOL is included. */
	QString toCsv(const QChar& separator = ',') const;


private:
	QString m_leftContext;
	QString m_match;
	QString m_rightMatch;
	QString m_rightContext;
	QString m_metadata;
	QString m_pageId;
	QUrl m_link;
	int m_page;
	QPoint m_position;
	QList<Highlight> m_highlights;
};
