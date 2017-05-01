/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf
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
	/** Remove all items. */
	void clear() {m_items.clear();}
private:
	int m_limit;
	QStringList m_items;
};

#endif // LOG_H
