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

#pragma once
#include <QList>

template<typename T> class History
{
public:
	History() {m_iterator = m_data.begin();}
	bool isEmpty() const {return m_data.isEmpty();}
	T current() const   {return m_iterator != m_data.end() ? *m_iterator : T();}
	bool hasPrevious() const {return m_iterator != m_data.begin();}
	T previous() const;
	bool hasNext() const {return m_iterator != m_data.end() && m_iterator != m_data.end() - 1;}
	T next() const;
	void forward();
	void back();
	void add(const T& t);
	void clear();
	void truncate();
	int count() const {return m_data.count();}
	void dump();

private:
	QList<T> m_data;
	typename QList<T>::iterator m_iterator;
};

template<typename T> T History<T>::previous() const
{
	if (hasPrevious())
		return *(m_iterator - 1);
	return T();
}

template<typename T> T History<T>::next() const
{
	if (hasNext())
		return *(m_iterator + 1);
	return T();
}

template<typename T> void History<T>::forward()
{
	if (hasNext())
		m_iterator++;
}

template<typename T> void History<T>::back()
{
	if (hasPrevious())
		m_iterator--;
}

template<typename T> void History<T>::add(const T& t)
{
	if (m_iterator != m_data.end() && *m_iterator == t)
		return;
	truncate();
	m_data.append(t);
	m_iterator = m_data.end() - 1;
}

template<typename T> void History<T>::clear()
{
	m_data.clear();
	m_iterator = m_data.begin();
}

template<typename T> void History<T>::truncate()
{
	if (m_iterator != m_data.end())
		m_data.erase(m_iterator + 1, m_data.end());
}

#include <QDebug>
template<typename T> void History<T>::dump()
{
	qDebug() << "History";
	for (int i = 0; i < count(); i++)
		qDebug() << m_data[i];
	qDebug() << "\n";
}
