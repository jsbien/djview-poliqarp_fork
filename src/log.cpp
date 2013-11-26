/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "log.h"

Log::Log()
{
	m_limit = 300;
}

void Log::addMessage(const QString& type, const QUrl& url)
{
	if (m_items.count() > m_limit)
		m_items.removeFirst();
	m_items.append(QString("%1: %2 %3")
					  .arg(QTime::currentTime().toString())
					  .arg(type).arg(url.toString()));
}

void Log::setLimit(int limit)
{
	m_limit = limit;
	while (m_items.count() > m_limit)
		m_items.removeFirst();
}
