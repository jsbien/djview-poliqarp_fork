/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef LOG_H
#define LOG_H

#include <QtCore>


class Log
{
public:
	Log();
	/** Add single message. */
	void addMessage(const QString& type, const QUrl& url);
	/** @return items. */
	QStringList items() const {return m_items;}
	/** Set item limit. */
	void setLimit(int limit);
	/** @return item limit. */
	int limit() const {return m_limit;}
private:
	int m_limit;
	QStringList m_items;
};

#endif // LOG_H
