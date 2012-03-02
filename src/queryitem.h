/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef QUERYITEM_H
#define QUERYITEM_H

#include <QtCore>

class QueryItem
{
public:
	 QueryItem();
	 void setLeftContext(const QString& context)	{m_leftContext = context;}
	 QString leftContext()	{return m_leftContext;}
	 void setRightContext(const QString& context)	{m_rightContext = context;}
	 QString rightContext()	{return m_rightContext;}
	 void setWord(const QString& word)	{m_word = word;}
	 QString word() const	{return m_word;}
	 void setLink(const QUrl& link);
	 QUrl link() const	{return m_link;}
	 int page() const {return m_page;}
	 QRect highlighted() const	{return m_highlighted;}
	 QPoint position() const {return m_position;}
private:
	 QString m_leftContext;
	 QString m_word;
	 QString m_rightContext;
	 QUrl m_link;
	 QRect m_highlighted;
	 int m_page;
	 QPoint m_position;
};

#endif // QUERYITEM_H
