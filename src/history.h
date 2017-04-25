/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
