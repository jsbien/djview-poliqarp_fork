/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#pragma once
#include <QLineEdit>
#include "replacements.h"

class ReplaceLineEdit : public QLineEdit
{
public:
	ReplaceLineEdit(QWidget* parent = nullptr);
   ~ReplaceLineEdit() = default;
	void configure();
protected:
	void keyPressEvent(QKeyEvent* event);

	/** @return first matching replacement for given suffix or empty replacement if there is none. */
	Replacement findReplacement(const QString& suffix) const;
	Replacements m_replacements;
};

