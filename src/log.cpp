/****************************************************************************
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
