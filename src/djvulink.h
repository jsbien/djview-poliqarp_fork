/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef DJVULINK_H
#define DJVULINK_H

#include <QtCore>

/** The DjVuLink class contains a link to external DjVu document
	 with optional information like context and highlighting. */
class DjVuLink
{
public:
	  DjVuLink();
	  DjVuLink(QUrl link)	{setLink(link);}
	  bool isValid() const {return m_link.isValid();}
	  int columns() const {return 4 - m_rightMatch.isEmpty();}
	  void setLeftContext(const QString& context)	{m_leftContext = context;}
	  QString leftContext()	{return m_leftContext;}
	  void setRightContext(const QString& context)	{m_rightContext = context;}
	  QString rightContext()	{return m_rightContext;}
	  void setMatch(const QString& word)	{m_match = word;}
	  QString match() const	{return m_match;}
	  void setRightMatch(const QString& word)	{m_rightMatch = word;}
	  QString rightMatch() const	{return m_rightMatch;}
	  void setLink(const QUrl& link);
	  QUrl link() const	{return m_link;}
	  QString documentPath() const;
	  int page() const {return m_page;}
	  QRect highlighted() const	{return m_highlighted;}
	  QPoint position() const {return m_position;}
	  QString text() const	{return m_leftContext + ' ' + m_match + ' ' + m_rightContext;}
	  void setMetadata(const QString& text) {m_metadata = text;}
	  QString metadata() const {return m_metadata;}
private:
	  QString m_leftContext;
	  QString m_match;
	  QString m_rightMatch;
	  QString m_rightContext;
	  QString m_metadata;
	  QUrl m_link;
	  QRect m_highlighted;
	  int m_page;
	  QPoint m_position;
};

#endif // QUERYITEM_H
