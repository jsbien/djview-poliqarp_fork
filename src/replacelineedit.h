/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#pragma once
#include <QLineEdit>

class ReplaceLineEdit : public QLineEdit
{
public:
	ReplaceLineEdit(QWidget* parent = 0);
	~ReplaceLineEdit();
	void configure();
protected:
	void keyPressEvent(QKeyEvent* event);


	struct Replacement {
		QString from;
		QString to;
		Replacement() {}
		Replacement(const QString& line);
		bool isValid() const;
		QString expand(const QString& previous, int cursor, const QString& suffix) const;
	};
	/** @return first matching replacement for given suffix or empty replacement if there is none. */
	Replacement findReplacement(const QString& suffix) const;

	QList<Replacement> m_replacements;
};

