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
	 QString leftContext()	{return m_rightContext;}
	 void setRightContext(const QString& context)	{m_rightContext = context;}
	 QString rightContext()	{return m_rightContext;}
	 void setWord(const QString& word)	{m_word = word;}
	 QString word() const	{return m_word;}
	 void setBookmark(const QUrl& bookmark)	{m_bookmark = bookmark;}
	 QUrl bookmark() const	{return m_bookmark;}
	 void setLink(const QUrl& link)	{m_link = link;}
	 QUrl link() const	{return m_link;}
private:
	 QString m_leftContext;
	 QString m_word;
	 QString m_rightContext;
	 QUrl m_bookmark;
	 QUrl m_link;
};

#endif // QUERYITEM_H
